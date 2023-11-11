var gateway = `ws://${window.location.hostname}/ws`;
var websocket;
var candiesStock = 0;

function initWebSocket() {
    console.log('Trying to open a WebSocket connection...');
    websocket = new WebSocket(gateway);
    websocket.onopen    = onOpen;
    websocket.onclose   = onClose;
    websocket.onmessage = onMessage; // <-- add this line
}

function onOpen(event) {
    console.log('Connection opened');
}

function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
}

function onMessage(event) {
    var state = document.getElementById('state')
    //verificar que el mensaje comience con "dispensing"
    var startsWith = event.data.split(":")[0];
    if (startsWith == "Dispensing"){
        var bool = event.data.split(":")[1];
        if (bool == "1"){
            state.innerHTML = "Dispensing...";
            state.classList.add("dispensing");
            state.classList.remove("ready");
        } else {
            state.innerHTML = "Ready";
            state.classList.add("ready");
            state.classList.remove("dispensing");
            document.getElementById('button').disabled = false;
        }
    }

    //verificar que el mensaje comience con "candies"
    else if (startsWith == "Candies"){
        candiesStock = event.data.split(":")[1];
        var stock = document.getElementById('candies stock');
        stock.innerHTML = candiesStock;
        document.getElementById('candies number').max = candiesStock;
    }
}

window.addEventListener('load', onLoad);

function onLoad(event) {
    initWebSocket();
    initButton();
    initStock(); 
}

function initButton() {
    document.getElementById('button').addEventListener('click', buy);
}

function initStock() {
    //Processor already iniatilices the stock
    candiesStock = document.getElementById('candies stock').innerHTML;
    //set max to input to buy
    document.getElementById('candies number').max = candiesStock;
}

function buy(){
    document.getElementById('button').disabled = true;
    var candiesBuyed = document.getElementById('candies number');
    websocket.send('buy:' + candiesBuyed.value);
    candiesBuyed.value = '';
}