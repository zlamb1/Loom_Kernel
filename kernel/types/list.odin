package types

import "kernel:cfg"

ListNode :: struct {
	prev: ^ListNode,
	next: ^ListNode,
}

@(private = "file")
ListIterator :: struct {
	list: ^ListNode,
	cur:  ^ListNode,
}

list_init :: proc "contextless" (self: ^ListNode) {
	cfg.dbg_assert(self != nil)

	self.prev = self
	self.next = self
}

list_iterator_create :: proc(list: ^ListNode) -> ListIterator {
	cfg.dbg_assert(list != nil)
	cfg.dbg_assert(list.prev != nil)
	cfg.dbg_assert(list.next != nil)

	return {list = list, cur = list.next}
}

list_iterate :: proc(iterator: ^ListIterator) -> (node: ^ListNode, ok: bool) {
	cfg.dbg_assert(iterator != nil)

	cur := iterator.cur

	if cur == iterator.list {
		ok = false
		return
	}

	iterator.cur = cur.next
	ok = true
	node = cur

	return
}

list_iterate_entries :: proc(
	iterator: ^ListIterator,
	$T: typeid,
	$field_name: string,
) -> (
	ptr: ^T,
	ok: bool,
) {
	node = list_iterate(iterator) or_return
	ptr = container_of(node, T, field_name)
	ok = true
	return
}

list_add :: list_prepend

list_prepend :: proc(list: ^ListNode, node: ^ListNode) {
	cfg.dbg_assert(list != nil)
	cfg.dbg_assert(list.prev != nil)
	cfg.dbg_assert(list.next != nil)

	node.prev = list
	node.next = list.next
	list.next.prev = node
	list.next = node
}

list_append :: proc(list: ^ListNode, node: ^ListNode) {
	cfg.dbg_assert(list != nil)
	cfg.dbg_assert(list.prev != nil)
	cfg.dbg_assert(list.next != nil)

	node.prev = list.prev
	node.next = list
	list.prev.next = node
	list.prev = node
}

list_remove :: proc(node: ^ListNode) {
	cfg.dbg_assert(node != nil)
	cfg.dbg_assert(node.prev != nil)
	cfg.dbg_assert(node.next != nil)

	node.prev.next = node.next
	node.next.prev = node.prev
	node.prev = node
	node.next = node
}

list_is_empty :: proc(list: ^ListNode) -> bool {
	cfg.dbg_assert(list != nil)
	cfg.dbg_assert(list.prev != nil)
	cfg.dbg_assert(list.next != nil)

	when cfg.DBG {
		if list == list.prev || list == list.next {
			cfg.dbg_assert(list.prev == list.next)
			return true
		} else do return false
	}

	return list == list.next
}
