async function loadOrders() {
	
	// const res = await fetch(...);
	const res = await fetch("http://127.0.0.1:8080/orders");

	const text = await res.text();
	const rows = text.split("\n");

	// Then prepare you document object

}

document.getElementById("refresh").addEventListener("click", (e) => {
	loadOrders();
});
