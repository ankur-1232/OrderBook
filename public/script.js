const el = id => document.getElementById(id);

el("place").addEventListener("click", async () => {
    const body = {
        side:  el("side").value,
        price: parseFloat(el("price").value),
        size:  parseFloat(el("size").value),
    };
    const res = await fetch("/order", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify(body),
    });
    const data = await res.json();
    console.log(data);
    refresh();
});

async function refresh() {
    const res  = await fetch("/orderbook");
    const book = await res.json();

    const rows = Math.max(book.bids.length, book.asks.length);
    const tbody = el("bids");
    tbody.innerHTML = "";

    for (let i = 0; i < rows; i++) {
        const tr = document.createElement("tr");
        const bid = book.bids[i];
        const ask = book.asks[i];
        tr.innerHTML = `
            <td>${bid ? bid.price : ""}</td>
            <td>${bid ? bid.size  : ""}</td>
            <td>${ask ? ask.price : ""}</td>
            <td>${ask ? ask.size  : ""}</td>`;
        tbody.appendChild(tr);
    }
}

refresh();   
