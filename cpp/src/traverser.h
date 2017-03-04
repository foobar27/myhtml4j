/*
 * This file is part of the myhtml4j distribution.
 *   (https://github.com/foobar27/myhtml4j)
 * Copyright (c) 2017 Sebastien Wagener.
 *
 * myhtml4j is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, version 3.
 *
 * myhtml4j is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#pragma once

#include <algorithm>

// Recursive pseudo-code for the following iterative implementation:
//    void transferSubTree(root) {
//        if (!visitor.pre(root)) {
//            return;
//        }
//        for (auto child : children) {
//            transferSubTree(child);
//        }
//        visitor.post(wc, root);
//    }

template<class NodeAdapter, class Visitor>
void traverse(Visitor& visitor, typename NodeAdapter::node_type root) {
    using namespace std;
    using Node = typename NodeAdapter::node_type;

    if (!root) {
        return; // nothing to do
    }
    vector<Node> stackA;
    vector<Node> stackB;
    stackA.push_back(root);
    while (!stackA.empty()) {
        auto n = stackA.back();
        stackA.pop_back();

        if (n) {
            if (!visitor.pre(n)) {
                continue;
            }
            stackB.push_back(n);

            // Add special token to stackA which will trigger a post-order visit
            stackA.push_back(nullptr);

            // Add children to stackA (reverse order)
            auto children = NodeAdapter().getChildren(n);
            copy(children.rbegin(), children.rend(), back_inserter(stackA));
        } else {
            visitor.post(stackB.back());
            stackB.pop_back();
        }
    }
}
