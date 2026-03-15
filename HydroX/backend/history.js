import express from "express";
import db from "../firebase.js";

const router = express.Router();

router.get("/month",async(req,res)=>{

 const snap = await db.ref("daily").once("value");

 const data = snap.val();

 let sum={f1:0,f2:0,f3:0,f4:0};

 for(let d in data){
  sum.f1+=data[d].f1;
  sum.f2+=data[d].f2;
  sum.f3+=data[d].f3;
  sum.f4+=data[d].f4;
 }

 res.json(sum);
});

export default router;
