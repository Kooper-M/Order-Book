import os
import sys
import json
import asyncio
import uvicorn
import requests
from os import getenv
from fastapi import FastAPI
from fastapi import HTTPException
from fastapi.responses import Response
from dotenv import load_dotenv
from pathlib import Path
from pydantic import BaseModel

# CMake places the native extension in its build directory. Uvicorn's reload
# worker only adds the project directory to sys.path, so include it explicitly.
#BUILD_DIR = Path(__file__).resolve().parent / "build" / "ucrt64-debug"
#sys.path.insert(0, str(BUILD_DIR))

import OrderBook

NGROK_URL = os.getenv("NGROK_URL")

app = FastAPI()
order_book = OrderBook.OrderBook()

class OrderRequest(BaseModel):
    buyOrSell: bool
    price: int
    quantity: int

@app.post("/orders")
async def create_order(buyOrSell: bool, price: int, quantity: int):
    order_book.insertOrder(buyOrSell, price, quantity)

@app.get("/orders")
async def get_order():
    return order_book.getOrders()

@app.get("/orders/{id}")
async def get_order(id: int):
    order = order_book.getOrder(id)

    if order is None:
        raise HTTPException(
            status_code=404,
            detail=f"Order {id} not found"
        )
    
    return order

@app.delete("/orders/{id}")
async def remove_order(id: int):
    return order_book.removeOrder(id)

@app.get("/trades") 
async def print_trades():
    return order_book.getTrades()

if __name__ == "__main__":
    uvicorn.run(app, host="0.0.0.0", port=8080)
