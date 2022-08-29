#pragma once

#include <cstdint>

template<typename T = uint64_t>
class AcpiTable {
public:
    virtual bool validate();

private:
	T* rootTable; // rsdt or xsdt
};