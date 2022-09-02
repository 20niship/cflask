const fetchWithTimeout = async (resource, options = {}) => {
  const { timeout = 8000 } = options;
  const controller = new AbortController();
  const id = setTimeout(() => controller.abort(), timeout);
  const response = await fetch(resource, {
    ...options,
    signal: controller.signal
  });
  clearTimeout(id);
  return response;
}

const post_fetch = async (uri, data = "") => {
  const res = await fetch(uri, {
    method: "POST",
    body: data
  })
  return res.ok;
}

const fetch_list = async (uri, list) => {
  let buf = new Uint8Array(list.length);
  i = 0;
  for (const b of list) { buf[i] = b; i += 1; }
  const res = await fetch(uri, { method: "POST", body: buf.buffer });
  return res.ok;
}

const ping = async (uri) => {
  i = 0;
  for (const b of list) { buf[i] = b; i += 1; }
  const res = await fetch(uri);
  return res.ok;
}

const fetch_data_to_string = async (uri, data) => {
  const res = await fetch(uri, { method: "POST", body: data.toString() });
  return res.ok;
}


function CustomDrawer(id = "visualizer") {
  let plotter = undefined;
  let plotting_data = [];

  function set_plotter(id) {
    plotter = new myPlot(id);
    plotter.set_field_size(12000, 12000);
    plotter.create_shader();
  }
  set_plotter(id);

  const MsgTypes = {
    PointCloudXX: 1,
    PointCloudYY: 2,
    Rectangle: 3,
    WiredRectange: 4,
    Circle: 5,
    WiredCircle: 6,
    Line: 7,
    LineList: 8,
    Triangle: 9,
    WiredTriangle: 10,

    Error: 11,
    Info: 12,
    Alert: 13,
    SetCanvasID: 14,
    JS: 15,
  };

  function parse(value) {
    const index = new Float32Array(value.buffer, 0, 1)[0];
    const draw_type = new Float32Array(value.buffer, 4, 1)[0];
    const col = new Float32Array(value.buffer, 8, 3);
    switch (draw_type) {
      case MsgTypes.PointCloudXX:
      case MsgTypes.PointCloudYY:
        {
          const size = new Float32Array(value.buffer, 20)[0] | 1;
          const pc_raw = new Float32Array(value.buffer, 24);
          let pc = []
          console.log(pc_raw.length)
          for (let i = 0; i < pc_raw.length / 2; i++) pc.push([pc_raw[i * 2], pc_raw[i * 2 + 1]]);
          plotting_data[index] = {
            type: MsgTypes.PointCloudXX,
            col, size, pc
          };
        }
        break;
      case MsgTypes.Rectangle:
      case MsgTypes.WiredRectange:
      case MsgTypes.Line:
        {
          const rest = new Float32Array(value.buffer, 20);
          const p1 = [rest[0], rest[1]];
          const p2 = [rest[2], rest[3]];
          const width = rest[4] | 10;
          plotting_data[index] = {
            type:draw_type, 
            col, p1, p2, width
          };
        }
        break;
      case MsgTypes.Circle:
      case MsgTypes.WiredCircle:
        {
          const rest = new Float32Array(value.buffer, 20);
          const p = [rest[0], rest[1]];
          const r = rest[2];
          const width= rest[3] | 2;
          plotting_data[index] = {
            type: draw_type,
            col, p, r, width 
          };
        }
        break;
      case MsgTypes.LineList:
        {
          const width = new Float32Array(value.buffer, 20, 1)[0] | 10;
          const pc_raw = new Float32Array(value.buffer, 24);
          let points = []
          for (let i = 0; i < pc_raw.length / 2; i++) points.push([pc_raw[i * 2], pc_raw[i * 2 + 1]]);
          plotting_data[index] = {
            type:draw_type, 
            col, points, width
          };
        }
        break;
      case MsgTypes.Triangle:
      case MsgTypes.WiredTriangle:
    }
    update_plot();
  }

  function update_plot() {
    plotter.clear_buffer();
    console.log(plotting_data)
    for (d of plotting_data) {
      switch (d?.type) {
        case MsgTypes.PointCloudYY:
        case MsgTypes.PointCloudXX:
          for (let p of d.pc) plotter.point(p, d.col, d.size)
          break;
        case MsgTypes.Rectangle:
          plotter.rectangle_top_bottom(d.p1, d.p2, d.col);
          break;
        case MsgTypes.WiredRectange:
          plotter.rectangle_top_bottom(d.p1, d.p2, d.col, d.width);
          break;
        case MsgTypes.Circle:
          plotter.circle(d.p, d.r, d.col);
          break;
        case MsgTypes.WiredCircle:
          plotter.wired_circle(d.p, d.r, d.col, d.width);
          break;
        case MsgTypes.Line:
          plotter.line(d.p1, d.p2, d.col, d.width);
          break;
        case MsgTypes.LineList:
          for (let i=0; i<d.points.length-1; i++){
            let p1 = d.points[i];
            let p2 = d.points[i+1];
            plotter.line(p1, p2, d.col, d.width);
          } 
          break;
        case MsgTypes.Triangle:
        case MsgTypes.WiredTriangle:
          plotter.wired_circle(d.center, d.r, d.col, d.width);
          break;
      }
    }
    plotter.update_buffers();
    plotter.draw();
  }
  function start_stream() {
    fetch('/stream', { method: "POST" })
      .then(response => {
        const reader = response.body.getReader();
        return new ReadableStream({
          start(controller) {
            const pump = async () => {
              return reader.read().then(({ done, value }) => {
                if (done) {
                  controller.close();
                  return;
                } else {
                  parse(value);
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
  start_stream();
}
