import express from "express";
import db from "../firebase.js";

const router = express.Router();
const RATE = 1500; // ₹ per 1000L

router.get("/",async(req,res)=>{

 const snap = await db.ref("daily").once("value");
 const data = snap.val();

 let cost={};

 for(let d in data){
  for(let f in data[d]){
   cost[f]=(cost[f]||0)+(data[d][f]/1000)*RATE;
  }
 }

 res.json(cost);
});

export default router;
