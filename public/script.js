let nextId = 1;

document.getElementById("place").addEventListener("click", async () => {
    const order = {
        id:       parseInt(document.getElementById("id").value),
        type:     document.getElementById("type").value,
        side:     document.getElementById("side").value,
        price:    parseFloat(document.getElementById("price").value),
        quantity: parseInt(document.getElementById("quantity").value)
    };

    await fetch("/order", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify(order)
    });

    nextId++;
    document.getElementById("id").value = nextId;
    fetchBook();
});

async function fetchBook() {
    const res  = await fetch("/orderbook");
    const data = await res.json();

    const bids = data.bids || [];
    const asks = data.asks || [];
    const rows = Math.max(bids.length, asks.length);

    const tbody = document.getElementById("book-body");
    tbody.innerHTML = "";

    for (let i = 0; i < rows; i++) {
        const tr = document.createElement("tr");

        // Bid side
        if (bids[i]) {
            tr.innerHTML += `<td>${bids[i].price}</td>
                             <td>${bids[i].quantity}</td>
                             <td>${bids[i].type}</td>`;
        } else {
            tr.innerHTML += `<td></td><td></td><td></td>`;
        }

        // Spacer
        tr.innerHTML += `<td></td>`;

        // Ask side
        if (asks[i]) {
            tr.innerHTML += `<td>${asks[i].price}</td>
                             <td>${asks[i].quantity}</td>
                             <td>${asks[i].type}</td>`;
        } else {
            tr.innerHTML += `<td></td><td></td><td></td>`;
        }

        tbody.appendChild(tr);
    }
}

// Poll every 2s
fetchBook();
setInterval(fetchBook, 2000);   
