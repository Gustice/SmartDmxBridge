/// API Routes GET
const ApiGetIntensity = "/api/getIntensity/";
const ApiSetIntensity = "/api/setIntensity";
const ApiGetColor = "/api/getColor/";
const ApiSetColor = "/api/setColor";

const ApiGetType = "/api/getType";
const ApiGetConfig = "/api/getConfig";
const ApiSetConfig = "/api/setConfig";


const http = new EasyHttp();

const Ui = (function () {
    const devConfigSpace = document.getElementById('showDeviceConfiguration');

    function evalDeviceConfig(response) {
        const caption = document.createElement('h3');
        caption.appendChild(document.createTextNode('Device Configuration'));
        const list = document.createElement('ul');
        
        const item1 = document.createElement('li');
        item1.appendChild(document.createTextNode(`Strip Type: ${response.OutputType}`));
        list.appendChild(item1);
        const item2 = document.createElement('li');
        item2.appendChild(document.createTextNode(`Color Type: ${response.ColorType}`));
        list.appendChild(item2);
        const item3 = document.createElement('li');
        item3.appendChild(document.createTextNode(`LED Count: ${response.LedCount}`));
        list.appendChild(item3);

        devConfigSpace.appendChild(caption);
        devConfigSpace.appendChild(list);
    }

    async function init() {
        // await http.get(ApiGetStatus_DeviceConfig)
        //     .then(data => evalDeviceConfig(data))
        //     .catch(err => console.log(err)); 
        }
    
    // type may be  emphasis ,userNote ,userWarning ,userError ,userSuccess ,
    function showMessage(message, type = 'userNote') {
        const basePoint = container;
        const parent = basePoint.parentElement;
        const now = new Date();

        const messageObj = document.createElement('div');
        messageObj.className = 'row ' + type;
        let stamp = 'VolatileNote' + now.getTime();
        messageObj.id = stamp;
        messageObj.appendChild(document.createTextNode(message));
        
        parent.insertBefore(messageObj, basePoint.nextSibling);
        setTimeout(function() {document.getElementById(stamp).remove()}, 3000);
    }
    
    function showFormMessage(anchor, message, type = 'userNote') {
        const basePoint = document.getElementById(anchor.id);
        const parent = basePoint.parentElement.parentElement;
        const now = new Date();
        const stamp = 'VolatileNote' + now.getTime();

        const messageObj = document.createElement('div');
        messageObj.className = 'row ' + type;
        messageObj.id = stamp;
        messageObj.appendChild(document.createTextNode(message));

        parent.insertBefore(messageObj, basePoint.parentElement.nextSibling);
        setTimeout(function () { document.getElementById(stamp).remove() }, 3000);
    }


    return {
        init: init,
        showMessage: showMessage,
        showFormMessage: showFormMessage
    }
})();

Ui.init();

class Color {
    constructor(red, green, blue) {
      this.red = red;
      this.green = green;
      this.blue = blue;
    }
  }

let FgColor = new Color(0,0,0);
let BgColor = new Color(0,0,0);

function toRgb(hex) {
    var result = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(hex);
    return result ? {
        r: parseInt(result[1], 16),
        g: parseInt(result[2], 16),
        b: parseInt(result[3], 16)
    } : null;
}
function hexVal(value) {
    return (+value).toString(16).padStart(2, '0');
}
function rgbHex(r, g, b) {
    return "#" + hexVal(r) + hexVal(g) + hexVal(b)
}
function rgb(r, g, b) {
    return "rgb(" + r + "," + g + "," + b + ")";
}

function updateColor(sender) {
    console.log(sender);
    let value = toRgb(sender.value);
    let name = sender.name;
    let color = new Color(value.r, value.g, value.b);

    if(name == "foreground") {
        FgColor = color;
        console.log("update foreground");
        console.log(color);
    } else if(name == "background") {
        BgColor = color;
        console.log("update background");
        console.log(color);
    }
    
    sender.preventDefault;
}

async function sendIntendities(form) {
    const formData = new FormData(form).entries();
    let jsonObject = {};

    for (const [key, value] of formData) {
        var formE = form.querySelector(`input[name="${key}"]`);
        if (formE.type == "number")
            jsonObject[key] = parseInt(value);
        else
            jsonObject[key] = value;
    }
    console.log(JSON.stringify(jsonObject));
    await http.post(ApiSetIntensity, jsonObject)
    .catch(err => console.log(err)); 
}

function onSubmitIntensities(form) {
    sendIntendities(form);
    Ui.showFormMessage(form, "Data sent ...", 'userSuccess');
    return false;
}

async function sendAmbient(form) {
    console.log("posting foreground");
    console.log(FgColor);
    
    let data = ambientColorToJson("foreground", FgColor);
    console.log(data);
    await http.post(ApiSetColor, data)
    .catch(err => console.log(err)); 
    
    console.log("posting foreground");
    console.log(FgColor);
    
    data = ambientColorToJson("background", BgColor);
    console.log(data);
    await http.post(ApiSetColor, data)
    .catch(err => console.log(err)); 

    Ui.showFormMessage(form, "Data sent ...", 'userSuccess');
}

function onSubmitAmbient(form) {
    sendAmbient(form);
    return false;
}

function ambientColorToJson(type, value) {
    let object = {};
    object["T"] = type;
    object["R"] = value.red;
    object["G"] = value.green;
    object["B"] = value.blue;
    return object;
}