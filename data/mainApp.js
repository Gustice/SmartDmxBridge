/// API Routes GET
const ApiGetIntensity = "/api/getIntensity";
const ApiSetIntensity = "/api/setIntensity";
const ApiGetColor = "/api/getColor";
const ApiSetColor = "/api/setColor";

const ApiGetType = "/api/getType";
const ApiGetConfig = "/api/getConfig";
const ApiSetConfig = "/api/setConfig";

const http = new EasyHttp();

class Color {
    constructor(red, green, blue) {
        this.red = red;
        this.green = green;
        this.blue = blue;
    }
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

function toRgb(hex) {
    var result = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(hex);
    return result ? {
        r: parseInt(result[1], 16),
        g: parseInt(result[2], 16),
        b: parseInt(result[3], 16)
    } : null;
}

const Ui = (function () {
    let fgColor = new Color(0, 0, 0);
    let bgColor = new Color(0, 0, 0);

    async function init() {
        await http.get(ApiGetConfig)
            .then(data => evalDeviceConfig(data))
            .catch(err => console.log(err));
        await http.get(ApiGetIntensity)
            .then(data => initIntensity(data))
            .catch(err => console.log(err));
        await http.get(ApiGetColor + "/foreground")
            .then(data => initAmbient(data, "fg"))
            .catch(err => console.log(err));
        await http.get(ApiGetColor + "/background")
            .then(data => initAmbient(data, "bg"))
            .catch(err => console.log(err));
    }

    function onSubmitIntensities(form) {
        sendIntensities(form);
        return false;
    }

    function onSubmitAmbient(form) {
        sendAmbient(form);
        return false;
    }

    function evalDeviceConfig(response) {
        // only log for so far
        console.log(response);
    }

    function initIntensity(data) {
        console.log("got data: ", data);
        document.getElementById("rIllum").value = data.I;
        document.getElementById("oIllum").value = data.I;
        document.getElementById("rAmbient").value = data.A;
        document.getElementById("oAmbient").value = data.A
    }

    function initAmbient(data, type) {
        console.log("got data: ", type, data);

        if (type == "fg") {
            document.getElementById("fgSelector").value = rgbHex(data.R, data.G, data.B)
        }

        if (type == "bg") {
            document.getElementById("bgSelector").value = rgbHex(data.R, data.G, data.B)
        }
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

    function ambientColorToJson(type, value) {
        let object = {};
        object["T"] = type;
        object["R"] = value.red;
        object["G"] = value.green;
        object["B"] = value.blue;
        return object;
    }

    function updateColor(sender) {
        let value = toRgb(sender.value);
        let name = sender.name;
        let color = new Color(value.r, value.g, value.b);

        if (name == "foreground") {
            fgColor = color;
            console.log("update foreground", color);
        } else if (name == "background") {
            bgColor = color;
            console.log("update background", color);
        }
        sender.preventDefault;
    }

    async function sendIntensities(form) {
        const formData = new FormData(form).entries();
        let jsonObject = {};

        for (const [key, value] of formData) {
            var formE = form.querySelector(`input[name="${key}"]`);
            if (formE.type == "number")
                jsonObject[key] = parseInt(value);
            else
                jsonObject[key] = value;
        }
        console.log("Update intensities", jsonObject);
        await http.post(ApiSetIntensity, jsonObject)
            .catch(err => console.log(err));

        showFormMessage(form, "Data sent ...", 'userSuccess');
    }

    async function sendAmbient(form) {
        console.log("posting foreground", fgColor);
        let data = ambientColorToJson("foreground", fgColor);
        console.log(data);
        await http.post(ApiSetColor, data)
            .catch(err => console.log(err));

        console.log("posting background", bgColor);
        data = ambientColorToJson("background", bgColor);
        console.log(data);
        await http.post(ApiSetColor, data)
            .catch(err => console.log(err));

        showFormMessage(form, "Data sent ...", 'userSuccess');
    }

    return {
        init: init,
        showFormMessage: showFormMessage,
        updateColor: updateColor,
        onSubmitIntensities: onSubmitIntensities,
        onSubmitAmbient: onSubmitAmbient
    }
})();

Ui.init();
