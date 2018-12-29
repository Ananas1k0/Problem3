#include "pch.h"

#include "Shop.h"
#include "Product.h"

#include <memory>

#include <iostream>
#include <atomic>
#include <thread>
#include <chrono>

using namespace std::chrono_literals;

void printShopLatPriceChange(IShopPtr shop, std::vector<IProductPtr> products)
{
	for (auto& good : products) {
		std::shared_ptr<PriceChange> pc = shop->GetPreviousPriceChange(good);

		if (pc != nullptr) {
			std::cout << shop->GetName() << ": last price changes for " << pc->productName << " old price = " << pc->oldPrice
				<< " new price = " << pc->newPrice << " " << pc->description << std::endl;
		}
	}
}


int main() {
	//create shops
	IShopPtr gracery = std::make_shared<Shop>("Gracery"); //can sale food and hardware
	IShopPtr bookshop = std::make_shared<Shop>("Bookshop"); //can sale books
	IShopPtr greengrocer = std::make_shared<Shop>("Greengrocer"); //can sale food
	IShopPtr hardwareShop = std::make_shared<Shop>("Hardware shop"); //can sale hardware
	IShopPtr supermarket = std::make_shared<Shop>("Supermarket"); //can sale food, hardware, books and gifts
	IShopPtr giftShop = std::make_shared<Shop>("GiftShop"); //can sale books and gifts

	//create products
	IProductPtr eggs = std::make_shared<Product>("Eggs", 100); //food
	IProductPtr milk = std::make_shared<Product>("Milk", 70); //food
	IProductPtr cookBook = std::make_shared<Product>("CookBook", 140); //book
	IProductPtr fantasyBook = std::make_shared<Product>("FantasyBook", 200); //book
	IProductPtr soop = std::make_shared<Product>("Soop", 40); //hardware
	IProductPtr washingPowder = std::make_shared<Product>("WashingPowder", 100); //hardware
	IProductPtr vase = std::make_shared<Product>("Vase", 300); //gifts
	IProductPtr beads = std::make_shared<Product>("Beads", 250); //gifts

	std::atomic<bool> isMarketOpen = true;

	auto trading = std::thread([&]() {
		eggs->StartSales();
		eggs->Attach(gracery);
		eggs->Attach(supermarket);

		milk->StartSales();
		milk->Attach(gracery);
		milk->Attach(greengrocer);
		milk->Attach(supermarket);

		cookBook->StartSales();
		cookBook->Attach(bookshop);
		cookBook->Attach(supermarket);
		cookBook->Attach(giftShop);

		fantasyBook->Attach(bookshop);
		fantasyBook->Attach(giftShop);
		fantasyBook->StartSales();

		soop->StartSales();
		soop->Attach(gracery);
		soop->Attach(supermarket);
		soop->Attach(hardwareShop);

		washingPowder->Attach(supermarket);
		washingPowder->Attach(hardwareShop);
		washingPowder->StartSales();

		vase->Attach(supermarket);
		vase->Attach(giftShop);
		vase->StartSales();

		beads->Attach(giftShop);
		beads->StartSales();

		std::this_thread::sleep_for(5s);

		eggs->Attach(greengrocer);
		eggs->ChangePrice(90);
		eggs->ChangePrice(80);
		milk->ChangePrice(80);
		milk->Detach(gracery);

		std::this_thread::sleep_for(5s);

		cookBook->Detach(supermarket);
		cookBook->ChangePrice(100);
		fantasyBook->ChangePrice(150);
		fantasyBook->Attach(supermarket);

		std::this_thread::sleep_for(5s);

		eggs->StopSales();
		soop->ChangePrice(50);
		soop->StopSales();

		std::this_thread::sleep_for(5s);

		eggs->ChangePrice(70);
		soop->StartSales();
		soop->ChangePrice(40);
		washingPowder->ChangePrice(120);
		fantasyBook->ChangePrice(200);
		beads->Detach(giftShop);
		vase->StopSales();

		std::this_thread::sleep_for(5s);

		eggs->StartSales();

		std::this_thread::sleep_for(5s);

		isMarketOpen = false;
	});

	while (isMarketOpen) {
		printShopLatPriceChange(gracery, { eggs, milk, soop, washingPowder});
		printShopLatPriceChange(bookshop, { cookBook, fantasyBook});
		printShopLatPriceChange(greengrocer, { eggs, milk });
		printShopLatPriceChange(hardwareShop, { soop, washingPowder });
		printShopLatPriceChange(supermarket, { eggs, milk, cookBook, fantasyBook, soop, washingPowder, vase, beads });
		printShopLatPriceChange(giftShop, { fantasyBook, cookBook, vase, beads });
		std::this_thread::sleep_for(1s);
	}

	trading.join();

	return 0;
}