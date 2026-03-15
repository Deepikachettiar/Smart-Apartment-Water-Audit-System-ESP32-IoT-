import axios from "axios";

const TOKEN = process.env.BLYNK_AUTH_TOKEN;

async function getLive(){

 const get = async(p)=>
  Number((await axios.get(
   `https://blynk.cloud/external/api/get?token=${TOKEN}&${p}`
  )).data);

 return {
  f1: await get("V0"),
  f2: await get("V1"),
  f3: await get("V2"),
  f4: await get("V3"),
  leak: await get("V10") // from ESP32
 };
}

export default getLive;
