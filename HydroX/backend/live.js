import express from "express";
import db from "../firebase.js";

const router = express.Router();

router.get("/", async (req, res) => {

  const snap = await db.ref("realtime").once("value");
  const data = snap.val();

  if (!data) {
    return res.json({
      f1: 0,
      f2: 0,
      f3: 0,
      f4: 0,
      total: 0
    });
  }

  const total =
    (data.f1 || 0) +
    (data.f2 || 0) +
    (data.f3 || 0) +
    (data.f4 || 0);

  res.json({
    f1: data.f1 || 0,
    f2: data.f2 || 0,
    f3: data.f3 || 0,
    f4: data.f4 || 0,
    total
  });

});

export default router;