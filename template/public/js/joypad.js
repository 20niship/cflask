class JoyPad{
  constructor(callback) {
    this.type;
    this.axis = [0,0,0,0];
    this.buttons = [0,0,0,0,0,0,0,0,0,0,0];
    this.name = "";
    this.connected = false;
    this.rAF = window.mozRequestAnimationFrame ||
        window.webkitRequestAnimationFrame ||
        window.requestAnimationFrame;
    this.callback = callback || function([], []) {};

    window.addEventListener("gamepadconnected", this.connecthandler);
    window.addEventListener("gamepaddisconnected", this.disconnecthandler);
}

update(){
    let pads = navigator.getGamepads ? navigator.getGamepads() : (navigator.webkitGetGamepads ? navigator.webkitGetGamepads : []);
    let pad = pads[0];
    if(!pad) return;
    let but = [];
    for(let i = 0 ; i < pad.buttons.length; i++) {
        var val = pad.buttons[i];
        var pressed = val == 1.0;
        if (typeof(val) == "object") {
            pressed = val.pressed;
            val     = val.value;
        }
        but[i] = val;
    }
    this.axis = pad.axes.concat();
    this.buttons = but.concat();
    if(this.axis.length > 4){
        const L = 11;
        this.buttons[L]   = pad.axes[4] == 1 ? 1 : 0;
        this.buttons[L+1] = pad.axes[4] == -1 ? 1 : 0;
        this.buttons[L+2] = pad.axes[5] == 1 ? 1 : 0;
        this.buttons[L+3] = pad.axes[5] == -1 ? 1 : 0;
        this.axis = [
            pad.axes[0],
            pad.axes[1],
            pad.axes[2],
            pad.axes[3],
        ]
    }
    this.buttons.length = 16;
    for(let i  = 0; i<this.axis.length; i++) this.axis[i] *= 100;
    this.callback(this.buttons, this.axis)
}

connecthandler(e){
  const gamepad = e?.gamepad | {};
    console.log("Gamepad Connected!!")
    this.name = gamepad?.id | "unknown";
    this.connected = true;
    window.addEventListener('joystickmove', e => this.update());
    window.addEventListener('buttonpress', e => showPressedButton(e.index));
    window.addEventListener('buttonrelease', e => removePressedButton(e.index));
    window.addEventListener('joystickmove', e => this.update()); 
}

disconnecthandler(e){
    console.log("Diconnected" + e.gamepad.index);
}
start(){
    setInterval(() => {this.update()}, 50);
    // this.rAF(() => {this.update()});
    window.addEventListener("gamepadconnected", this.connecthandler);
    window.addEventListener("gamepaddisconnected", this.disconnecthandler);
}
}

