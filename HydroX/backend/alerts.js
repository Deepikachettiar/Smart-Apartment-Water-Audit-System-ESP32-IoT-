import express from "express";
import db from "../firebase.js";

const router = express.Router();

/**
 * Get ONLY active leak alerts
 */
router.get("/", async (req, res) => {

  try {

    const snap = await db.ref("alerts").once("value");

    const data = snap.val();

    if (!data) return res.json([]);

    const alerts = Object.entries(data)
      .map(([id, v]) => ({ id, ...v }))
      .filter(a => a.type === "LEAK" && a.status === "ACTIVE")
      .sort((a, b) => b.time - a.time); // latest first

    res.json(alerts);

  } catch (err) {

    console.error("Alert API error:", err);
    res.status(500).json([]);

  }

});

export default router;
