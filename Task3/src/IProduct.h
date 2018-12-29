#pragma once

#include <memory>
#include <string>
#include <unordered_map> 

#include "IShop.h"

using IShopPtr = std::shared_ptr<class IShop>;
using Shops = std::unordered_map<std::string, IShopPtr>;

class IProduct : public std::enable_shared_from_this<IProduct> {
public:
	virtual void Attach(IShopPtr shop) = 0;
	virtual void Detach(IShopPtr shop) = 0;
	virtual double GetPrice() const = 0;
	virtual void ChangePrice(double price) = 0;
	virtual const std::string& GetName() const = 0;
	virtual void StartSales() = 0;
	virtual void StopSales() = 0;
	virtual ~IProduct() = default;
};