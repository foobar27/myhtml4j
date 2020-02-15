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
#include "adapter.h"

std::vector<lxb_dom_node_t*> MyHtmlAdapter::getChildren(lxb_dom_node_t* node) {
    std::vector<lxb_dom_node_t*> children;
    if (node->local_name == LXB_TAG_TEMPLATE) {
      auto temp = lxb_html_interface_template(node);
      if (temp->content) {
	return getChildren(&temp->content->node);
      }
    } else {
        auto child = node->first_child;
        while (child) {
	    children.push_back(child);
	    child = child->next;
	}
    }
    return children;
}
