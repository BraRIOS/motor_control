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
    //actualizar el estado de la maquina
    var startsWith = event.data.split(":")[0];
    if (startsWith == "Dispensing"){
        var bool = event.data.split(":")[1];
        if (bool == "1"){
            state.innerHTML = "Dispensing...";
            state.classList.add("dispensing");
            state.classList.remove("ready");
            document.getElementById('button').disabled = true;
        } else {
            state.innerHTML = "Ready";
            state.classList.add("ready");
            state.classList.remove("dispensing");
            document.getElementById('button').disabled = false;
        }
    }
    //actualizar el stock de candies
    else if (startsWith == "Candies"){
        candiesStock = event.data.split(":")[1];
        var stock = document.getElementById('candies stock');
        stock.innerHTML = candiesStock;
        document.getElementById('candies number').max = candiesStock;
    }
    //actualizar el precio
    else if (startsWith == "Price"){
        var price = document.getElementById('candies price');
        price.innerHTML = event.data.split(":")[1];
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
    var candiesBuyed = document.getElementById('candies number');
    validateInputCandies(candiesBuyed);
    document.getElementById('button').disabled = true;
    websocket.send('buy:' + candiesBuyed.value);
    candiesBuyed.value = '';
}

function validateInputCandies(input) {
    var value = parseInt(input.value);
    var max = parseInt(input.max);
    var error = document.getElementById('error');
    if (value > max) {
        error.innerHTML = 'Not enough candies in stock, max: ' + max + ' candies';
        error.style.display = 'block';
        document.getElementById('button').disabled = true;
        return;
    }
    else if(value == 0){
        error.innerHTML = 'You must buy at least one candy';
        error.style.display = 'block';
        document.getElementById('button').disabled = true;
        return;
    }
    else if(value < 0){
        error.innerHTML = 'You must buy a positive number of candies';
        error.style.display = 'block';
        document.getElementById('button').disabled = true;
        return;
    }
    else if(isNaN(value)){
        error.style.display = 'block';
        error.innerHTML = 'You must buy a number of candies';
        document.getElementById('button').disabled = true;
        return;
    }
    else {
        document.getElementById('button').disabled = false;
        error.style.display = 'none';
    }
}