#include "pch.h"
#include "Shop.h"
#include "Product.h"

TEST(BasicTests, TestShopName) {
	IShopPtr gracery = std::make_shared<Shop>("Gracery");

	EXPECT_EQ(gracery->GetName(), "Gracery");
}

TEST(BasicTests, TestProductNameAndPrice) {
	IProductPtr eggs = std::make_shared<Product>("Eggs", 100);

	EXPECT_EQ(eggs->GetName(), "Eggs");
	EXPECT_EQ(eggs->GetPrice(), 100);
}

TEST(BasicTests, TestShopClose) {
	IShopPtr gracery = std::make_shared<Shop>("Gracery");

	EXPECT_FALSE(gracery->isClose());

	gracery->Close();

	EXPECT_TRUE(gracery->isClose());
}

TEST(BasicTests, TestShopMethodsParameters1) {
	IShopPtr gracery = std::make_shared<Shop>("Gracery");

	bool res = false;

	try {
		gracery->UpdateProduct(nullptr);
	}
	catch (std::invalid_argument e) {
		res = true;
	}

	EXPECT_TRUE(res);
}

TEST(BasicTests, TestShopMethodsParameters2) {
	IShopPtr gracery = std::make_shared<Shop>("Gracery");
	IProductPtr eggs = std::make_shared<Product>("Eggs", -1);

	bool res = false;

	try {
		gracery->UpdateProduct(eggs);
	}
	catch (std::invalid_argument e) {
		res = true;
	}

	EXPECT_TRUE(res);
}

TEST(BasicTests, TestShopMethodsParameters3) {
	IShopPtr gracery = std::make_shared<Shop>("Gracery");
	IProductPtr eggs = std::make_shared<Product>("", 100);

	bool res = false;

	try {
		gracery->UpdateProduct(eggs);
	}
	catch (std::invalid_argument e) {
		res = true;
	}

	EXPECT_TRUE(res);
}

TEST(BasicTests, TestGetPriceChangeWithoutAttach) {
	IShopPtr gracery = std::make_shared<Shop>("Gracery");
	IProductPtr eggs = std::make_shared<Product>("Eggs", 100);

	EXPECT_EQ(gracery->GetPreviousPriceChange(eggs), nullptr);
}

TEST(BasicTests, TestGetPriceChangeFromClosedShop) {
	IShopPtr gracery = std::make_shared<Shop>("Gracery");
	IProductPtr eggs = std::make_shared<Product>("Eggs", 100);

	eggs->StartSales();
	eggs->Attach(gracery);

	gracery->Close();

	EXPECT_EQ(gracery->GetPreviousPriceChange(eggs), nullptr);
}

TEST(BasicTests, TestGetPriceChangeOpenShopFirstTime) {
	IShopPtr gracery = std::make_shared<Shop>("Gracery");
	IProductPtr eggs = std::make_shared<Product>("Eggs", 100);

	eggs->StartSales();
	eggs->Attach(gracery);

	std::shared_ptr<PriceChange> pc = gracery->GetPreviousPriceChange(eggs);

	EXPECT_NE(pc, nullptr);
	EXPECT_EQ(pc->newPrice, 100);
	EXPECT_EQ(pc->oldPrice, 0);
	EXPECT_EQ(pc->productName, "Eggs");
	EXPECT_EQ(pc->description, "FIRST_PRICE");
}

TEST(BasicTests, TestGetPriceChangeAfterDetach) {
	IShopPtr gracery = std::make_shared<Shop>("Gracery");
	IProductPtr eggs = std::make_shared<Product>("Eggs", 100);

	eggs->StartSales();
	eggs->Attach(gracery);
	eggs->ChangePrice(90);

	std::shared_ptr<PriceChange> pc = gracery->GetPreviousPriceChange(eggs);

	EXPECT_NE(pc, nullptr);
	EXPECT_EQ(pc->newPrice, 90);
	EXPECT_EQ(pc->oldPrice, 100);
	EXPECT_EQ(pc->productName, "Eggs");
	EXPECT_EQ(pc->description, "PRICE_CHANGED");
}

TEST(BasicTests, TestShopGetPriceAfterDetach) {
	IShopPtr gracery = std::make_shared<Shop>("Gracery");
	IProductPtr eggs = std::make_shared<Product>("Eggs", 100);

	eggs->StartSales();
	eggs->Attach(gracery);
	eggs->ChangePrice(90);
	eggs->Detach(gracery);

	std::shared_ptr<PriceChange> pc = gracery->GetPreviousPriceChange(eggs);

	EXPECT_EQ(pc, nullptr);
}

TEST(BasicTests, TestShopGetPriceAfterDetachAtach) {
	IShopPtr gracery = std::make_shared<Shop>("Gracery");
	IProductPtr eggs = std::make_shared<Product>("Eggs", 100);

	eggs->StartSales();
	eggs->Attach(gracery);
	eggs->ChangePrice(90);
	eggs->Detach(gracery);

	std::shared_ptr<PriceChange> pc1 = gracery->GetPreviousPriceChange(eggs);

	EXPECT_EQ(pc1, nullptr);

	eggs->Attach(gracery);

	std::shared_ptr<PriceChange> pc2 = gracery->GetPreviousPriceChange(eggs);

	EXPECT_NE(pc2, nullptr);
	EXPECT_EQ(pc2->newPrice, 90);
	EXPECT_EQ(pc2->oldPrice, 100);
	EXPECT_EQ(pc2->productName, "Eggs");
	EXPECT_EQ(pc2->description, "PRICE_CHANGED");
}

TEST(BasicTests, TestShopGetPriceAfterSalesStopingStarting) {
	IShopPtr gracery = std::make_shared<Shop>("Gracery");
	IProductPtr eggs = std::make_shared<Product>("Eggs", 100);

	eggs->StartSales();
	eggs->Attach(gracery);
	eggs->ChangePrice(90);
	eggs->StopSales();

	std::shared_ptr<PriceChange> pc1 = gracery->GetPreviousPriceChange(eggs);

	EXPECT_EQ(pc1, nullptr);

	eggs->StartSales();

	std::shared_ptr<PriceChange> pc2 = gracery->GetPreviousPriceChange(eggs);

	EXPECT_NE(pc2, nullptr);
	EXPECT_EQ(pc2->newPrice, 90);
	EXPECT_EQ(pc2->oldPrice, 100);
	EXPECT_EQ(pc2->productName, "Eggs");
	EXPECT_EQ(pc2->description, "PRICE_CHANGED");
}

TEST(BasicTests, TestShopGetPriceMultipleGoods) {
	IShopPtr gracery = std::make_shared<Shop>("Gracery");
	IProductPtr eggs = std::make_shared<Product>("Eggs", 100);
	IProductPtr milk = std::make_shared<Product>("Milk", 200);

	eggs->StartSales();
	eggs->Attach(gracery);

	milk->StartSales();
	milk->Attach(gracery);

	std::shared_ptr<PriceChange> pc1 = gracery->GetPreviousPriceChange(eggs);

	EXPECT_NE(pc1, nullptr);
	EXPECT_EQ(pc1->newPrice, 100);
	EXPECT_EQ(pc1->oldPrice, 0);
	EXPECT_EQ(pc1->productName, "Eggs");
	EXPECT_EQ(pc1->description, "FIRST_PRICE");

	std::shared_ptr<PriceChange> pc2 = gracery->GetPreviousPriceChange(milk);

	EXPECT_NE(pc2, nullptr);
	EXPECT_EQ(pc2->newPrice, 200);
	EXPECT_EQ(pc2->oldPrice, 0);
	EXPECT_EQ(pc2->productName, "Milk");
	EXPECT_EQ(pc2->description, "FIRST_PRICE");
}

