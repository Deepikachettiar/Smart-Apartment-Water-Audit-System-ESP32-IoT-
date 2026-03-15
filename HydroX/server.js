import dotenv from "dotenv";
import express from "express";
import cors from "cors";
import db from "./firebase.js";
import getLive from "./blynk.js";
import liveRoutes from "./routes/live.js";
import historyRoutes from "./routes/history.js";
import alertRoutes from "./routes/alerts.js";
import billRoutes from "./routes/billing.js";

dotenv.config();

const app = express();
app.use(cors());

app.use("/api/live", liveRoutes);
app.use("/api/history", historyRoutes);
app.use("/api/alerts", alertRoutes);
app.use("/api/billing", billRoutes);


/******************** LEAK TRACKING ********************/

let flowStreak = {
  f1: 0,
  f2: 0,
  f3: 0,
  f4: 0
};

let lastLeak = {
  f1: false,
  f2: false,
  f3: false,
  f4: false
};

const LEAK_THRESHOLD = 0.05;   // L/s
const REQUIRED_STREAK = 20;   // 20 * 3s = 60s


/******************** REALTIME POLLER ********************/

setInterval(async () => {

  const d = await getLive();


  /************ BACKEND LEAK DETECTION ************/

  const flows = {
    f1: d.f1,
    f2: d.f2,
    f3: d.f3,
    f4: d.f4
  };

  for (const flat in flows) {

    if (flows[flat] > LEAK_THRESHOLD) {
      flowStreak[flat]++;
    } 
    else {
      flowStreak[flat] = 0;
      lastLeak[flat] = false;
    }

    // Leak detected after 60s
    if (flowStreak[flat] >= REQUIRED_STREAK && !lastLeak[flat]) {

      await db.ref("alerts").push({
  time: Date.now(),
  type: "LEAK",
  flat: flat,
  message: "Continuous water flow for 1 minute",
  status: "ACTIVE"
});


      console.log(`🚨 Leak detected in ${flat}`);

      lastLeak[flat] = true;
    }
  }


  /************ USAGE CALCULATION ************/

  const liters = {
    f1: d.f1 * 3,
    f2: d.f2 * 3,
    f3: d.f3 * 3,
    f4: d.f4 * 3
  };

  await db.ref("realtime").set(liters);


  /************ DAILY TOTAL ************/

  const today = new Date().toISOString().slice(0, 10);

  await db.ref("daily/" + today).transaction(old => {

    old ||= { f1: 0, f2: 0, f3: 0, f4: 0 };

    return {
      f1: old.f1 + liters.f1,
      f2: old.f2 + liters.f2,
      f3: old.f3 + liters.f3,
      f4: old.f4 + liters.f4
    };

  });

}, 3000);


/******************** SERVER ********************/

app.listen(3000, () => {
  console.log("Server started on port http://localhost:3000");
});