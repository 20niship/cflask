const connection_checker = async () => {
  const check_ping = async (url) => {
    try {
      const response = await fetchWithTimeout(url, { timeout: 1000 });
      return response.ok;
    } catch (error) {
      return false;
    }
  }
  const set_status = (id, ok, name) => {
    let el = document.getElementById(id);
    if (ok) {
      el.classList.add("status-alive");
      el.classList.remove("status-dead");
    } else {
      el.classList.add("status-dead");
      el.classList.remove("status-alive");
      MyMessage({ msg: "Down" + name + "?", duration: 500 })
    }
  }
  set_status("indicator-r2", await check_ping("/ping"), "R2");
  set_status("indicator-r1", await check_ping("http://192.168.1.22:9000"), "R1");
  // set_status("indicator-cognition", await check_ping("http://192.168.1.23:5050"), "C");
}
const is_r2 = () => {
  const url = new URL(window.location.href).href;
  console.log(url);
  return url.includes("192.168.1.23");
}

const arraysEqual = (a, b) => {
  if (a === b) return true;
  if (a == null || b == null) return false;
  if (a.length !== b.length) return false;
  for (var i = 0; i < a.length; ++i) {
    if (a[i] !== b[i]) return false;
  }
  return true;
}

const start_joypad = (joypad_id, draw = false) => {
  let joypad_buf = [];

  let dots = {};
  const drawJoystick = (values, left) => {
    const DOT_SIZE = 10;
    let id = (left ? 'left' : 'right');
    let canvas = document.getElementById(id + '-joystick');
    let ctx = canvas.getContext('2d');
    if (dots[id]) {
      ctx.clearRect(dots[id].x - 1, dots[id].y - 1, DOT_SIZE + 2, DOT_SIZE + 2);
    }
    let x = Math.ceil(canvas.width / 2.8 * (1 + values[0] / 100));
    let y = Math.ceil(canvas.height / 2.8 * (1 + values[1] / 100));
    let rect = { x: x - DOT_SIZE / 2, y: y - DOT_SIZE / 2 };
    ctx.fillStyle = 'cyan';
    ctx.fillRect(rect.x, rect.y, DOT_SIZE, DOT_SIZE);
    dots[id] = rect;
  }

  const display_joypad_buttons = (buttons) => {
    const els = [
      document.getElementById("js-btn-cross"),
      document.getElementById("js-btn-circle"),
      document.getElementById("js-btn-rect"),
      document.getElementById("js-btn-triangle"),
      document.getElementById("js-btn-l1"),
      document.getElementById("js-btn-r1"),
      document.getElementById("js-btn-l2"),
      document.getElementById("js-btn-r2"),
      document.getElementById("js-btn-select"),
      document.getElementById("js-btn-start"),
      document.getElementById("js-btn-l3"),
      document.getElementById("js-btn-r3"),
      document.getElementById("js-btn-top"),
      document.getElementById("js-btn-bottom"),
      document.getElementById("js-btn-left"),
      document.getElementById("js-btn-right"),
    ];
    let i = 0;
    for (const b of buttons) {
      el = els[i];
      if (!el) return;
      if (b > 0) {
        el.classList.remove("active-joypad-btn");
      } else {
        el.classList.add("active-joypad-btn");
      }
      i += 1;
    }
  }
  let joypad = new JoyPad(async(buttons, axis) => {
    i = 0;
    if (draw) {
      drawJoystick([axis[0], axis[1]], true);
      drawJoystick([axis[2], axis[3]], false);
    }
    let new_data = []
    for (const b of buttons) { new_data[i] = b; i += 1; }
    for (const b of axis) { new_data[i] = b + 100; i += 1; }
    if (!arraysEqual(joypad_buf, new_data)) {
      joypad_buf = new_data;
      let buf = new Uint8Array(20);
      i = 0;
      for (const b of buttons) { buf[i] = b; i += 1; }
      for (const b of axis) { buf[i] = b + 100; i += 1; }
      console.log(new String(buf))
     const start_time = new Date();
      await fetch("/sync/joypad" + joypad_id.toString(), {
        method: "POST",
        body: buf.buffer
      })
        const end_time = new Date();
        document.getElementById("status-timer").textContent = (end_time - start_time).toString() + "ms";
    if(draw){
      display_joypad_buttons(buttons);
    }
      console.log("sennd joystick Data");
    }
  });
  joypad.start();
}


const start_js_stream = async() => {
  const decoder = new TextDecoder();
fetch('/jsstream', {method:"POST"})
  .then(response => {
    const reader = response.body.getReader();
    return new ReadableStream({
      start(controller) {
      const pump = ()=> {
        return reader.read().then(({ done, value }) => {
          if (done) {
            controller.close();
            return;
          }else{
            console.log(value.buffer);
            const text = decoder.decode(value, {});
            console.log("js_stream get=>> ", text );
            eval(text);
          }
          controller.enqueue(value);
          return pump();
        });
      }
      return pump();
      }
    })
  })
  .then(stream => new Response(stream))
  .then(response => response.blob())
  .then(blob => URL.createObjectURL(blob))
  .catch(err => console.error(err));
}

const logger_to_btm = () => {
 const el = document.getElementById("logger");
  el.scrollTop = el.scrollHeight;
  el.scrollTo(0, 9999999999);
}
const log_clear = () => {  document.getElementById("logger").innerHTML =  "";}
const log_info = (msg) => {  document.getElementById("logger").innerHTML += `<p class="log-info"> ${msg} </p>`; logger_to_btm();}
const log_debug= (msg) => {  document.getElementById("logger").innerHTML += `<p class="log-debug"> ${msg} </p>`; logger_to_btm();}
const log_warning = (msg) => {  document.getElementById("logger").innerHTML += `<p class="log-warning"> ${msg} </p>`; logger_to_btm();}
const log_error = (msg) => {  document.getElementById("logger").innerHTML += `<p class="log-error"> ${msg} </p>`; logger_to_btm();}
