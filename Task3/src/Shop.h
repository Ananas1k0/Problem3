#pragma once

#include <atomic>
#include <string>
#include <unordered_map>
#include <mutex>

#include "IShop.h"

class Shop : public IShop {

public:

	Shop(std::string name) : name(name)
	{}

	bool isClose() {
		return isClosed;
	}

	void Close() {
		isClosed = true;

		std::lock_guard<std::mutex> guard(mutex);

		for (std::pair<std::string, IProductPtr> element : goods)
		{
			element.second->Detach(shared_from_this());
		}
	}

	const std::string& GetName() const {
		return name;
	}

	void AddProduct(IProductPtr product) {
		if (isClosed) {
			return;
		}

		std::lock_guard<std::mutex> guard(mutex);

		validateProduct(product);

		updateLastPrices(product);

		goods.insert({ product->GetName(), product });
	}

	void UpdateProduct(IProductPtr product) {
		if (isClosed) {
			return;
		}

		std::lock_guard<std::mutex> guard(mutex);

		validateProduct(product);

		auto gIt = goods.find(product->GetName());

		if (gIt == goods.end()) {
			return;
		}

		updateLastPrices(product);

		goods.insert({ product->GetName(), product});
	}

	void RemoveProduct(IProductPtr product) {
		if (isClosed) {
			return;
		}

		std::lock_guard<std::mutex> guard(mutex);

		validateProduct(product);

		goods.erase( product->GetName());
	};

	const std::shared_ptr<PriceChange> GetPreviousPriceChange(IProductPtr product) {
		if (isClosed) {
			return nullptr;
		}

		std::lock_guard<std::mutex> guard(mutex);

		validateProduct(product);

		auto gIt = goods.find(product->GetName());

		if (gIt == goods.end()) {
			return nullptr;
		}

		auto pIt = lastPriceChanges.find(product->GetName());

		if (pIt != lastPriceChanges.end()) {
			std::shared_ptr<PriceChange> current = pIt->second;

			return current;
		}
		else {
			return nullptr;
		}
	};

private:

	//should be called under lock
	void validateProduct(IProductPtr product) {
		if (product == nullptr) {
			throw std::invalid_argument("Product is null.");
		}

		if (product->GetName().empty()) {
			throw std::invalid_argument("Product name wasn't set.");
		}

		if (product->GetPrice() <= 0) {
			throw std::invalid_argument("Product price is negative or zero.");
		}
	}

	//should be called under lock
	void updateLastPrices(IProductPtr product) {
		auto it = lastPriceChanges.find(product->GetName());

		if (it != lastPriceChanges.end()) {
			if (it->second->newPrice != product->GetPrice()) {
				std::shared_ptr<PriceChange> current = it->second;
				current->oldPrice = current->newPrice;
				current->newPrice = product->GetPrice();
				current->description = "PRICE_CHANGED";
			}
		}
		else {
			std::shared_ptr<PriceChange> current = std::make_shared<PriceChange>(PriceChange{ product->GetName(), 0, product->GetPrice() , "FIRST_PRICE"});
			lastPriceChanges.insert({ product->GetName(), current });
		}
	};

	Goods goods;
	std::unordered_map<std::string, std::shared_ptr<PriceChange>> lastPriceChanges;
	std::string name;
	std::atomic<bool> isClosed{ false };
	mutable std::mutex mutex;
};