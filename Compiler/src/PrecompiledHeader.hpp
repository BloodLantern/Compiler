#pragma once

#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <optional>
#include <unordered_map>
#include <variant>
#include <vector>

#include <magic_enum/magic_enum.hpp>

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Value.h>

#include "Attributes.hpp"
#include "CompilerSpecific.hpp"
#include "Position.hpp"
