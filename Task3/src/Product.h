#pragma once

#include <atomic>
#include <string>
#include <mutex>

#include "IProduct.h"

class Product : public IProduct {
public:
	Product(std::string name, double price) : name(name), price(price)
	{};

	void Attach(IShopPtr shop) {
		std::lock_guard<std::mutex> guard(mutex);

		validateShop(shop);

		auto it = shops.find(shop->GetName());

		if (it == shops.end()) {
			shops.insert({ shop->GetName(), shop });
			if (isTrading) {
				shop->AddProduct(shared_from_this());
			}
		}
	}

	void Detach(IShopPtr shop) {
		std::lock_guard<std::mutex> guard(mutex);

		validateShop(shop);

		auto it = shops.find(shop->GetName());

		if (it != shops.end()) {
			shops.erase(it);
			if (isTrading) {
				shop->RemoveProduct(shared_from_this());
			}
		}
	}

	double GetPrice() const {
		return price;
	}

	void ChangePrice(double price) {
		this->price = price;

		if (isTrading) {
			std::lock_guard<std::mutex> guard(mutex);

			for (std::pair<std::string, IShopPtr> element : shops)
			{
				element.second->UpdateProduct(shared_from_this());
			}
		}
	}

	const std::string& GetName() const {
		return name;
	}

	void StartSales() {
		if (!isTrading) {
			std::lock_guard<std::mutex> guard(mutex);

			for (std::pair<std::string, IShopPtr> element : shops)
			{
				element.second->AddProduct(shared_from_this());
			}

			isTrading = true;
		}
	}

	void StopSales() {
		if (isTrading) {
			std::lock_guard<std::mutex> guard(mutex);

			for (std::pair<std::string, IShopPtr> element : shops)
			{
				element.second->RemoveProduct(shared_from_this());
			}

			isTrading = false;
		}
	}

private:

	void validateShop(IShopPtr shop) {
		if (shop == nullptr) {
			throw std::invalid_argument("Shop is null.");
		}

		if (shop->GetName().empty()) {
			throw std::invalid_argument("Shop name wasn't set.");
		}
	}


	std::string name;
	std::atomic<double> price;
	Shops shops;
	std::atomic<bool> isTrading{ false };
	mutable std::mutex mutex;

};