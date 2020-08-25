/*
 Andrew Chang
 Inventory Allocator
 Deliverr Recruitment
 */

#include <unordered_map>
#include <string>
#include <vector>
#include <iostream>
#include <cassert>
using namespace std;

/* equivalent to storing something like this: { dm: { apple: 5 }} */
struct Delivery {
    Delivery(string name) {
        nameOfWareHouse = name;
    }
    string nameOfWareHouse;
    unordered_map<string,int> itemsToDeliver;
};

/* equivalent to storing something like this: { name: owd, inventory: { apple: 5, orange: 10 } } */
struct WareHouse {
    WareHouse(string n, unordered_map<string,int> inv) {
        name = n;
        inventory = inv;
    }
    string name;
    unordered_map<string,int> inventory;
};

class InventoryAllocator {
public:
    vector<Delivery*> cheapestShipment (unordered_map<string,int> order, vector<WareHouse> w) {
        unordered_map<string, Delivery*> wareHouseToDelivery; /* maps warehouse name to delivery object */
        unordered_map<string, int> availableAmt; /* maps item name (string) to amount available so far */
        vector<Delivery*> result;
        /* Go through each item in order, checking each warehouse until required amount is met.
         We assume that the warehouses are already sorted in ascending order of cost. */
        for (auto it = order.begin(); it != order.end(); it++) {
            string itemRequested = it->first;
            int requestedAmount = it->second;
            vector<Delivery*> deliveriesForThisItem;
            
            /* iterate over each warehouse*/
            for (WareHouse house : w) {
                unordered_map<string,int> wareHouseInventory = house.inventory;
                /* check if the warehouse has this item */
                if (wareHouseInventory.find(itemRequested) != wareHouseInventory.end()) {
                    Delivery* d = new Delivery(house.name);
                    if (availableAmt[itemRequested] + wareHouseInventory[itemRequested] >= requestedAmount) {
                        d->itemsToDeliver[itemRequested] = requestedAmount - availableAmt[itemRequested]; /* we have extra inventory */
                        availableAmt[itemRequested] = requestedAmount;
                    }
                    else {
                        d->itemsToDeliver[itemRequested] = wareHouseInventory[itemRequested];
                        availableAmt[itemRequested] += wareHouseInventory[itemRequested];
                    }
                    deliveriesForThisItem.push_back(d);
                }
                if (availableAmt[itemRequested] == requestedAmount) {
                    break;
                }
            }
            /* If not enough inventory, return empty vector */
            if (availableAmt[itemRequested] < requestedAmount) {
                return result;
            }

            /* Then, either mege with deliveries associated with same warehouse, or associate a warehouse with this delivery */
            for (Delivery* d : deliveriesForThisItem) {
                string wareHouseName = d->nameOfWareHouse;
                if (wareHouseToDelivery.find(wareHouseName) == wareHouseToDelivery.end()) {
                    wareHouseToDelivery[wareHouseName] = d; /* we haven't added this warehouse before */
                }
                else {
                    /* we need to pick up another item at a warehouse we already visited */
                    unordered_map<string,int> additionalItemsToDeliver = d->itemsToDeliver;
                    for (auto it2 = additionalItemsToDeliver.begin(); it2 != additionalItemsToDeliver.end(); it2++) {
                        string item = it2->first;
                        int amt = it2->second;
                        wareHouseToDelivery[wareHouseName]->itemsToDeliver[item] = amt;
                    }
                    free(d);
                }
            }
        }
                
        /* Output final delivery, if inventory is sufficient. Order doesn't matter. */
        
        for (auto it = wareHouseToDelivery.begin(); it != wareHouseToDelivery.end(); it++) {
            result.push_back(it->second);
        }
        return result;
    }
    
};

void freeDeliveries(vector<Delivery*> result) {
    for (Delivery* d : result) {
        free(d); /* memory management */
    }
}

int main() {
    
    unordered_map<string,int> Order1;
    Order1["apple"] = 1;
    
    unordered_map<string,int> Order2;
    Order2["apple"] = 10;
    
    unordered_map<string,int> Order3;
    Order3["apple"] = 7;
    unordered_map<string,int> Order4;
    unordered_map<string,int> Order5;
    Order5["apple"] = 5;
    Order5["banana"] = 5;
    
    unordered_map<string,int> Inventory1;
    Inventory1["apple"] = 1;
    unordered_map<string,int> Inventory2;
    Inventory2["apple"] = 0;
    unordered_map<string,int> Inventory3;
    Inventory3["apple"] = 5;
    unordered_map<string,int> Inventory4;
    unordered_map<string,int> Inventory5;
    Inventory5["banana"] = 5;
    
    WareHouse wh1("a", Inventory1);
    WareHouse wh2("b", Inventory2);
    WareHouse wh3("c", Inventory3);
    WareHouse wh3b("c2", Inventory3);
    WareHouse wh4("d", Inventory4);
    WareHouse wh5("e", Inventory5);
    
    vector<WareHouse> houseList1;
    houseList1.push_back(wh1);
    
    vector<WareHouse> houseList2;
    houseList2.push_back(wh2);
    
    vector<WareHouse> houseList3;
    houseList3.push_back(wh3);
    houseList3.push_back(wh3b);
    
    vector<WareHouse> houseList4;
    houseList4.push_back(wh4);
    
    vector<WareHouse> houseList5;
    houseList5.push_back(wh3);
    houseList5.push_back(wh5);
    
    InventoryAllocator i;
    
    cout << "TEST CASE 1: EXACT MATCH" << endl;
    vector<Delivery*> res = i.cheapestShipment(Order1, houseList1);
    assert(res.size() == 1);
    assert(res[0]->itemsToDeliver["apple"] == 1);
    freeDeliveries(res);
    
    cout << "TEST CASE 2: NOT ENOUGH INVENTORY" << endl;
    res = i.cheapestShipment(Order1, houseList2);
    assert(res.size() == 0);
    
    cout << "TEST CASE 3: EMPTY ORDER" << endl;
    res = i.cheapestShipment(Order4, houseList1);
    assert(res.size() == 0);
    
    cout << "TEST CASE 4: NO INVENTORY AT ALL" << endl;
    res = i.cheapestShipment(Order1, houseList4);
    assert(res.size() == 0);
    
    cout << "TEST CASE 5: SPLIT ITEM ACROSS WAREHOUSE" << endl;
    res = i.cheapestShipment(Order2, houseList3);
    assert(res.size() == 2);
    assert(res[0]->itemsToDeliver["apple"] + res[1]->itemsToDeliver["apple"] == 10);
    freeDeliveries(res);
    
    cout << "TEST CASE 6: SPLIT ITEM ACROSS WAREHOUSE, EXTRA INVENTORY" << endl;
    res = i.cheapestShipment(Order3, houseList3);
    assert(res.size() == 2);
    assert(res[0]->itemsToDeliver["apple"] + res[1]->itemsToDeliver["apple"] == 7);
    freeDeliveries(res);
    
    cout << "TEST CASE 7: MULTIPLE FOODS, ENOUGH INVENTORY" << endl;
    res = i.cheapestShipment(Order5, houseList5);
    assert(res.size() == 2);
    assert( (res[0]->itemsToDeliver["apple"] + res[1]->itemsToDeliver["banana"] == 10) ||
            (res[1]->itemsToDeliver["apple"] + res[0]->itemsToDeliver["banana"] == 10)
           );
    freeDeliveries(res);
    
    cout << "TEST CASE 8: MULTIPLE FOODS, NOT ENOUGH INVENTORY" << endl;
    res = i.cheapestShipment(Order5, houseList3);
    assert(res.size() == 0);
    
    cout << "ALL TEST CASES PASSED" << endl;
    
    return 0;
}

