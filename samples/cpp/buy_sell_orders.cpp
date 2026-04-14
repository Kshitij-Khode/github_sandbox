#include <iostream>
#include <optional>
#include <vector>
#include <algorithm>

using namespace std;

class Book {
private:
    vector<tuple<int, int>> buyOrders;
    vector<tuple<int, int>> sellOrders;
    int orderId = 0;

    optional<tuple<int, int>> matchMake() {
        for (auto buyOrderIter = buyOrders.begin(); buyOrderIter != buyOrders.end(); ++buyOrderIter) {
            const auto [buyPrice, buyId] = *buyOrderIter;
            for (auto sellOrderIter = sellOrders.begin(); sellOrderIter != sellOrders.end(); ++sellOrderIter) {
                const auto [sellPrice, sellId] = *sellOrderIter;
                if (buyPrice >= -sellPrice) {
                    buyOrders.erase(buyOrderIter);
                    sellOrders.erase(sellOrderIter);
                    return tuple<int, int>(buyId, sellId);
                }
            }
        }
        return nullopt;
    }

public:
    Book() {}

    optional<int> getBestBuyOrder() {
        if (buyOrders.size() == 0) return nullopt;
        return std::get<1>(buyOrders[0]);
    }

    optional<int> getBestSellOrder() {
        if (sellOrders.size() == 0) return nullopt;
        return -(std::get<1>(sellOrders[0]));
    }

    optional<tuple<int, int>> placeOrder(string type, int val) {
        if (type == "buy") {
            buyOrders.push_back(tuple<int, int>(val, orderId));
            push_heap(buyOrders.begin(), buyOrders.end());
            orderId++;
            return matchMake();
        }
        if (type == "sell") {
            sellOrders.push_back(tuple<int, int>(-val, orderId));
            push_heap(sellOrders.begin(), sellOrders.end());
            orderId++;
            return matchMake();
        }
        return nullopt;
    }

    void printBuyList() {
        for (const auto& [price, id]: buyOrders) {
            cout << price << ", " << id << endl;
        }
    }

    void printSellList() {
        for (const auto& [price, id]: sellOrders) {
            cout << price << ", " << id << endl;
        }
    }
};

int main() {
    Book book;
    if (auto temp = book.placeOrder("buy", 5)) {
        cout << get<0>(*temp) << ", " << get<1>(*temp) << endl;
    }
    if (auto temp = book.placeOrder("buy", 9)) {
        cout << get<0>(*temp) << ", " << get<1>(*temp) << endl;
    }
    if (auto temp = book.placeOrder("sell", 5)) {
        cout << get<0>(*temp) << ", " << get<1>(*temp) << endl;
    }
    if (auto temp = book.placeOrder("sell", 9)) {
        cout << get<0>(*temp) << ", " << get<1>(*temp) << endl;
    }
    return 0;
}
