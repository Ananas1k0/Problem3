#pragma once

#include <memory>
#include <string>
#include <unordered_map> 

#include "IProduct.h"

using IProductPtr = std::shared_ptr<class IProduct>;
using Goods = std::unordered_map<std::string, IProductPtr>;

struct PriceChange {
	std::string productName;
	double oldPrice;
	double newPrice;
	std::string description;
};


class IShop : public std::enable_shared_from_this<IShop> {
public:

	virtual const std::string& GetName() const = 0;
	virtual bool isClose() = 0;
	virtual void Close() = 0;
	virtual void AddProduct(IProductPtr product) = 0;
	virtual void UpdateProduct(IProductPtr product) = 0;
	virtual void RemoveProduct(IProductPtr product) = 0;
	const virtual std::shared_ptr<PriceChange> GetPreviousPriceChange(IProductPtr product) = 0;
};