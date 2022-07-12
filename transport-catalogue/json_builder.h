#pragma once

#include "json.h"

#include <vector>
#include <string>

namespace json {

    class DictItemContext;
    class KeyItemContext;
    class KeyValueItemContext;
    class ArrayItemContext;
    class Builder;

    class KeyValueItemContext {
    private:
        Builder& builder_;

    public:
        KeyValueItemContext(Builder& builder) : builder_(builder) {};
        KeyItemContext& Key(std::string);
        Builder& EndDict();
    };

    class KeyItemContext {
    private:
        Builder& builder_;

    public:
        KeyItemContext(Builder& builder) : builder_(builder) {};
        KeyValueItemContext& Value(Node::Value);
        DictItemContext& StartDict();
        ArrayItemContext& StartArray();
    };

    class DictItemContext {
    private:
        Builder& builder_;

    public:
        DictItemContext(Builder& builder) : builder_(builder) {};
        KeyItemContext& Key(std::string);
        Builder& EndDict();
    };

    class ArrayItemContext {
    private:
        Builder& builder_;

    public:
        ArrayItemContext(Builder& builder) : builder_(builder) {};
        ArrayItemContext& Value(Node::Value);
        DictItemContext& StartDict();
        ArrayItemContext& StartArray();
        Builder& EndArray();
    };

    class Builder {
    private:
        Node root_;
        std::vector<Node*> nodes_stack_;
        bool key_opened_ = 0;
        std::string last_key_;

    public:
        Builder& Value(Node::Value);
        DictItemContext& StartDict();
        ArrayItemContext& StartArray();
        Node& Build();
        //
        Builder &EndDict();
        Builder& EndArray();
        Builder &Key(std::string);
    };
}