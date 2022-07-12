#include "json_builder.h"
#include "json.h"
#include <vector>
#include <utility>

namespace json {
    Builder &Builder::Value(Node::Value value) {
        Node node(value);
        if (nodes_stack_.size() == 0) {//store simple value in root
            if (!root_.IsNull()) {
                throw std::logic_error("Value error: Trying to set root value twice");
            }
            root_.GetValue() = value;
            return *this;
        }
        if (nodes_stack_.back()->IsDict()) {  //dict value processing
            if (!key_opened_) {
                throw std::logic_error("Value error: Can't setting value of dict without leading key");
            }
            Node& last_value = nodes_stack_.back()->AsDict()[last_key_];
            if (!last_value.IsNull()) {
                throw std::logic_error("StartArray: Double value for one key definition");
            }
            //set value for last key
            last_value.GetValue() = value;
            key_opened_ = 0;
            last_key_.clear();

        } else if (nodes_stack_.back()->IsArray()) {  //array value processing
            //add value to array
            nodes_stack_.back()->AsArray().push_back(std::move(node));
        } else {
            throw std::logic_error("Value error: Unknown structure on building stack");
        }
        return *this;
    }

    ArrayItemContext &Builder::StartArray() {
        if (nodes_stack_.size() == 0) {//store array value in root
            if (!root_.IsNull()) {
                throw std::logic_error("StartArray error: Trying to set root value twice");
            }
            root_.GetValue() = Array{};
            nodes_stack_.push_back(&root_);
            return *new ArrayItemContext(*this);
        }
        if (!nodes_stack_.empty() && nodes_stack_.back()->IsDict()) {  //dict value processing
            if (!key_opened_) {
                throw std::logic_error("StartArray: Can't setting value of dict without leading key");
            }
            Node& last_value = nodes_stack_.back()->AsDict()[last_key_];
            if (!last_value.IsNull()) {
                throw std::logic_error("StartArray: Double value for one key definition");
            }
            //set building array as value for last key
            last_value.GetValue() = Array{};
            key_opened_ = 0;
            last_key_.clear();
            //push pointer to array on building stack
            nodes_stack_.push_back(&last_value);

        } else if (!nodes_stack_.empty() && nodes_stack_.back()->IsArray()) {
            auto &building_array = nodes_stack_.back()->AsArray();
            //set building dict as value for previous array
            building_array.push_back(Node{Array{}});
            //push pointer to dict on building stack
            nodes_stack_.emplace_back(&building_array.back());

        }
        return *new ArrayItemContext(*this);
    }

    DictItemContext &Builder::StartDict() {
        if (nodes_stack_.size() == 0) {//store array value in root
            if (!root_.IsNull()) {
                throw std::logic_error("StartDict error: Trying to set root value twice");
            }
            root_.GetValue() = Dict{};
            nodes_stack_.push_back(&root_);
            return *new DictItemContext(*this);
        }
        if (!nodes_stack_.empty() && nodes_stack_.back()->IsDict()) {  //dict value processing
            if (!key_opened_) {
                throw std::logic_error("StartArray: Can't setting value of dict without leading key");
            }
            Node& last_value = nodes_stack_.back()->AsDict()[last_key_];
            if (!last_value.IsNull()) {
                throw std::logic_error("StartArray: Double value for one key definition");
            }
            //set building dict as value for last key - inplace
            last_value.GetValue() = Dict{};
            key_opened_ = 0;
            last_key_.clear();
            //push pointer to dict on building stack
            nodes_stack_.push_back(&last_value);

        } else if (!nodes_stack_.empty() && nodes_stack_.back()->IsArray()) {
            auto &building_array = nodes_stack_.back()->AsArray();
            //set building dict as value for previous array
            building_array.push_back(Node{Dict{}});
            //push pointer to dict on building stack
            nodes_stack_.emplace_back(&building_array.back());
        }
        return *new DictItemContext(*this);
    }

    Builder &Builder::EndArray() {
        if (nodes_stack_.empty()) {
            throw std::logic_error("EndDict error: No array to build");
        }
        if (!nodes_stack_.empty() && !nodes_stack_.back()->IsArray()) {
            throw std::logic_error("EndArray error: No array to build");
        }
        //
        if (nodes_stack_.size() == 1) {
            //building root array - store it into root
            root_ = *nodes_stack_.back();
        }
        if (!nodes_stack_.empty()) {
            nodes_stack_.pop_back();
        }
        return *this;
    }

    Builder &Builder::EndDict() {
        if (nodes_stack_.empty()) {
            throw std::logic_error("EndDict error: No dictionary to build");
        }
        if (!nodes_stack_.empty() && !nodes_stack_.back()->IsDict()) {
            throw std::logic_error("EndDict error: No dictionary to build");
        }
        //
        if (nodes_stack_.size() == 1) {
            //building root dict - store it into root
            root_ = *nodes_stack_.back();
        }
        if (!nodes_stack_.empty()) {
            nodes_stack_.pop_back();
        }
        return *this;
    }

    Builder &Builder::Key(std::string key) {
        if (nodes_stack_.empty()) {
            throw std::logic_error("Key error: definition out of dictionary");
        }
        if (!nodes_stack_.back()->IsDict()) {
            throw std::logic_error("Key error: definition out of dictionary");
        }
        if (key_opened_) {
            throw std::logic_error("Key: Sequence keys definition without value");
        }
        Node node{};
        nodes_stack_.back()->AsDict()[key] = std::move(node);
        key_opened_ = 1;
        last_key_ = key;
        return *this;
    }

    Node &Builder::Build() {
        if (root_.IsNull()) {
            throw std::logic_error("Build: root is empty");
        }
        if (!nodes_stack_.empty() && nodes_stack_.back()->IsDict())  {
            throw std::logic_error("Build: AsDict not finished");
        }
        if (!nodes_stack_.empty() && nodes_stack_.back()->IsArray())  {
            throw std::logic_error("Build: Array not finished");
        }
        if (key_opened_) {
            throw std::logic_error("Build: AsDict key without value");
        }
        return root_;
    }

    KeyItemContext& DictItemContext::Key(std::string key) {
        builder_ = builder_.Key(key);
        return *new KeyItemContext(builder_);
    }

    Builder& DictItemContext::EndDict() {
        return builder_.EndDict();
    }

    KeyValueItemContext& KeyItemContext::Value(Node::Value value) {
        builder_ = builder_.Value(value);
        return *new KeyValueItemContext(builder_);
    }

    DictItemContext& KeyItemContext::StartDict() {
        return builder_.StartDict();

    }

    ArrayItemContext& KeyItemContext::StartArray() {
        return builder_.StartArray();
    }

    KeyItemContext& KeyValueItemContext::Key(std::string key) {
        builder_ = builder_.Key(key);
        return *new KeyItemContext(builder_);
    }

    Builder& KeyValueItemContext::EndDict() {
        return builder_.EndDict();
    }
    //ArrayItemContext
    ArrayItemContext& ArrayItemContext::Value(Node::Value value) {
        builder_ = builder_.Value(value);
        return *new ArrayItemContext(builder_);
    }

    DictItemContext& ArrayItemContext::StartDict() {
        return builder_.StartDict();

    }

    ArrayItemContext& ArrayItemContext::StartArray() {
        return builder_.StartArray();
    }

    Builder& ArrayItemContext::EndArray() {
        return builder_.EndArray();
    }
}