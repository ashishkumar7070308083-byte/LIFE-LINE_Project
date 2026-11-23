const canvas = document.getElementById("cityMapCanvas");
const ctx = canvas.getContext("2d");

const zones = [
    { id: 0, name: "CGC Trauma Hub", x: 95, y: 195 },
    { id: 1, name: "Mohali Phase-7", x: 300, y: 85 },
    { id: 2, name: "Kharar Junction", x: 300, y: 305 },
    { id: 3, name: "PGI / Sec-12 Chd", x: 525, y: 195 },
    { id: 4, name: "ISBT Sec-17 Chd", x: 685, y: 195 }
];

const roads = [
    { u: 0, v: 1, w: 5 },
    { u: 0, v: 2, w: 6 },
    { u: 2, v: 1, w: 3 },
    { u: 1, v: 3, w: 7 },
    { u: 2, v: 3, w: 9 },
    { u: 3, v: 4, w: 6 }
];

const ambulances = {
    "AMB-CH01": "Driver: Gurpreet_Singh | Zone: 0 (CGC Trauma Hub) | Status: Available",
    "AMB-CH02": "Driver: Simran_Kaur | Zone: 2 (Kharar Junction) | Status: Available",
    "AMB-CH03": "Driver: Harpreet_Gill | Zone: 1 (Mohali Phase-7) | Status: Dispatched"
};

const hospitals = [
    { id: "HOSP-CH1", name: "PGI Sector-12 Trauma Center", zone: 3, beds: 36, icu: 12 },
    { id: "HOSP-CH2", name: "CGC Emergency Care Unit", zone: 0, beds: 22, icu: 6 },
    { id: "HOSP-CH3", name: "Mohali Civil & Care Hospital", zone: 1, beds: 18, icu: 5 }
];

let emergencies = [
    { id: "INC-1", severity: 10, patient: "Amandeep_Kaur", zone: 4, issue: "Cardiac_Arrest" },
    { id: "INC-3", severity: 9, patient: "Jaspreet_Sidhu", zone: 3, issue: "Fire_Burn" },
    { id: "INC-2", severity: 7, patient: "Karan_Sharma", zone: 2, issue: "Road_Accident" },
    { id: "INC-4", severity: 4, patient: "Neha_Verma", zone: 1, issue: "Fracture" }
];

const mstEdges = new Set([
    "0-1", "1-0",
    "1-2", "2-1",
    "1-3", "3-1",
    "3-4", "4-3"
]);

let dispatchLogs = [];
let nextIncNum = 5;
let showMST = false;
let currentActiveEdges = new Set();
let currentDestZone = -1;
let animFrameId = null;
let ambulancePos = null;

function checkCppStateSync() {
    fetch("state.json")
        .then(res => res.ok ? res.json() : null)
        .then(data => {
            if (data && data.status) {
                document.getElementById("cppSyncBadge").innerText = "C++ STATE SYNCED";
            }
        })
        .catch(() => {});
}

function sortBySeverityDesc() {
    emergencies.sort((a, b) => b.severity - a.severity);
}

function renderQueue() {
    sortBySeverityDesc();
    const container = document.getElementById("queueContainer");
    if (emergencies.length === 0) {
        container.innerHTML = `<div class="info-box">No pending emergencies in queue.</div>`;
        return;
    }
    container.innerHTML = emergencies.map(item => `
        <div class="queue-card">
            <div>
                <strong>${item.id}</strong> • ${item.patient} (${item.issue})<br>
                <small style="color:#94a3b8;">Target: ${zones[item.zone].name}</small>
            </div>
            <span class="sev-tag">Sev ${item.severity}/10</span>
        </div>
    `).join("");
}

function addIncident() {
    const rawPatient = document.getElementById("patientInput").value.trim();
    const zone = parseInt(document.getElementById("zoneSelect").value, 10);
    const sevInput = parseInt(document.getElementById("severityInput").value, 10);
    const severity = Number.isNaN(sevInput) ? 5 : Math.max(1, Math.min(10, sevInput));

    const parts = rawPatient ? rawPatient.split(/\s+/) : ["Patient_" + nextIncNum, "Emergency"];
    const patient = parts[0];
    const issue = parts.slice(1).join("_") || "Medical_Alert";

    emergencies.push({
        id: "INC-" + (nextIncNum++),
        severity,
        patient,
        zone,
        issue
    });

    document.getElementById("patientInput").value = "";
    renderQueue();
}

function dijkstra(src, dest) {
    const n = zones.length;
    const dist = Array(n).fill(Infinity);
    const parent = Array(n).fill(-1);
    const visited = Array(n).fill(false);

    dist[src] = 0;

    for (let count = 0; count < n; count++) {
        let u = -1;
        for (let i = 0; i < n; i++) {
            if (!visited[i] && (u === -1 || dist[i] < dist[u])) {
                u = i;
            }
        }
        if (u === -1 || dist[u] === Infinity) break;
        visited[u] = true;

        roads.forEach(edge => {
            if (edge.u === u && dist[u] + edge.w < dist[edge.v]) {
                dist[edge.v] = dist[u] + edge.w;
                parent[edge.v] = u;
            }
            if (edge.v === u && dist[u] + edge.w < dist[edge.u]) {
                dist[edge.u] = dist[u] + edge.w;
                parent[edge.u] = u;
            }
        });
    }

    const path = [];
    const activeEdges = new Set();
    for (let curr = dest; curr !== -1; curr = parent[curr]) {
        path.unshift(curr);
        if (parent[curr] !== -1) {
            activeEdges.add(`${parent[curr]}-${curr}`);
            activeEdges.add(`${curr}-${parent[curr]}`);
        }
    }

    return { distance: dist[dest], path, activeEdges };
}

function animateAmbulanceAlongPath(pathNodeIds) {
    if (animFrameId) {
        cancelAnimationFrame(animFrameId);
    }
    if (!pathNodeIds || pathNodeIds.length < 2) {
        ambulancePos = null;
        drawMap();
        return;
    }

    let segmentIndex = 0;
    let progress = 0;
    const speed = 0.025;

    function step() {
        const startZone = zones[pathNodeIds[segmentIndex]];
        const endZone = zones[pathNodeIds[segmentIndex + 1]];

        ambulancePos = {
            x: startZone.x + (endZone.x - startZone.x) * progress,
            y: startZone.y + (endZone.y - startZone.y) * progress
        };

        drawMap();

        progress += speed;
        if (progress >= 1) {
            progress = 0;
            segmentIndex++;
            if (segmentIndex >= pathNodeIds.length - 1) {
                ambulancePos = { x: endZone.x, y: endZone.y };
                drawMap();
                return;
            }
        }
        animFrameId = requestAnimationFrame(step);
    }

    animFrameId = requestAnimationFrame(step);
}

function dispatchHighest() {
    sortBySeverityDesc();
    if (emergencies.length === 0) {
        document.getElementById("routeSummary").innerHTML = "Queue is empty. Add an emergency first.";
        return;
    }

    const topCase = emergencies.shift();
    renderQueue();

    const result = dijkstra(0, topCase.zone);
    currentActiveEdges = result.activeEdges;
    currentDestZone = topCase.zone;

    const routeText = result.path.map(id => `${zones[id].name} [${id}]`).join(" ==> ");
    document.getElementById("routeSummary").innerHTML = `
        <strong>Dispatched ${topCase.id}</strong> (${topCase.patient} — ${topCase.issue}, Severity ${topCase.severity}/10)<br>
        <strong>Estimated Time:</strong> ${result.distance} mins | <strong>Green Corridor:</strong> ${routeText}
    `;

    dispatchLogs.push(`${topCase.id} (${topCase.patient}, Sev ${topCase.severity}) -> ${zones[topCase.zone].name} (${result.distance}m)`);
    renderLogs();
    animateAmbulanceAlongPath(result.path);
}

function toggleMST() {
    showMST = !showMST;
    if (showMST) {
        document.getElementById("routeSummary").innerHTML =
            `<strong>Prim's MST Active:</strong> Minimum Emergency Corridor Backbone highlighted in teal (Total Cost: 21 mins).`;
    }
    drawMap();
}

function lookupAmbulance() {
    const key = document.getElementById("ambLookupInput").value.trim().toUpperCase();
    const resBox = document.getElementById("ambResult");
    resBox.innerText = ambulances[key] || `ID "${key}" not found in Hash Table.`;
}

function renderHospitals() {
    const list = document.getElementById("hospitalList");
    list.innerHTML = hospitals.map(h => `
        <li><strong>${h.name}</strong> (Zone ${h.zone}) — ${h.beds} Beds, ${h.icu} ICU</li>
    `).join("");
}

function renderLogs() {
    const list = document.getElementById("logList");
    if (dispatchLogs.length === 0) {
        list.innerHTML = `<li>No dispatches logged yet.</li>`;
        return;
    }
    list.innerHTML = dispatchLogs.map(entry => `<li>${entry}</li>`).join("");
}

function undoLastLog() {
    if (dispatchLogs.length > 0) {
        dispatchLogs.pop();
        renderLogs();
    }
}

function drawMap() {
    ctx.clearRect(0, 0, canvas.width, canvas.height);

    roads.forEach(edge => {
        const a = zones[edge.u];
        const b = zones[edge.v];
        const key = `${edge.u}-${edge.v}`;
        const isDijkstraEdge = currentActiveEdges.has(key);
        const isMSTEdge = showMST && mstEdges.has(key);

        ctx.beginPath();
        ctx.moveTo(a.x, a.y);
        ctx.lineTo(b.x, b.y);

        if (isDijkstraEdge) {
            ctx.strokeStyle = "#10b981";
            ctx.lineWidth = 5;
        } else if (isMSTEdge) {
            ctx.strokeStyle = "#2dd4bf";
            ctx.lineWidth = 4;
        } else {
            ctx.strokeStyle = "#475569";
            ctx.lineWidth = 2;
        }
        ctx.stroke();

        const midX = (a.x + b.x) / 2;
        const midY = (a.y + b.y) / 2;
        ctx.fillStyle = isDijkstraEdge ? "#6ee7b7" : (isMSTEdge ? "#5eead4" : "#facc15");
        ctx.font = "bold 13px sans-serif";
        ctx.fillText(`${edge.w} min`, midX + 6, midY - 6);
    });

    zones.forEach(z => {
        ctx.beginPath();
        ctx.arc(z.x, z.y, 17, 0, Math.PI * 2);
        if (z.id === 0) {
            ctx.fillStyle = "#ef4444";
        } else if (z.id === currentDestZone) {
            ctx.fillStyle = "#10b981";
        } else {
            ctx.fillStyle = "#0284c7";
        }
        ctx.fill();
        ctx.lineWidth = 2.5;
        ctx.strokeStyle = "#f8fafc";
        ctx.stroke();

        ctx.fillStyle = "#f8fafc";
        ctx.font = "12px sans-serif";
        ctx.fillText(`[${z.id}] ${z.name}`, z.x - 45, z.y + 32);
    });

    if (ambulancePos) {
        ctx.beginPath();
        ctx.arc(ambulancePos.x, ambulancePos.y, 9, 0, Math.PI * 2);
        ctx.fillStyle = "#facc15";
        ctx.fill();
        ctx.lineWidth = 2.5;
        ctx.strokeStyle = "#111827";
        ctx.stroke();
    }
}

checkCppStateSync();
renderQueue();
renderHospitals();
renderLogs();
lookupAmbulance();
drawMap();