/* This header file implements RB tree, an internal data structure for MtStl. */

namespace MyStl
{
    enum _RB_tree_color {
        _red = 0,
        _black = 1
    };

    class _RB_tree_node_base {
        public:
            typedef _RB_tree_node_base* _Base_ptr;
            typedef const _RB_tree_node_base* _Const_Base_ptr;

            _Base_ptr _parent;
            _Base_ptr _left;
            _Base_ptr _right;
            _RB_tree_color _color;

            static _Base_ptr _min(_Base_ptr ptr) {
                while (ptr->_left != nullptr)
                    ptr = ptr->_left;
                return ptr;
            }

            static _Const_Base_ptr _min(_Const_Base_ptr ptr) {
                while (ptr->_left != nullptr)
                    ptr = ptr->_left;
                return ptr;
            }

            static _Base_ptr _max(_Base_ptr ptr) {
                while (ptr->_right != nullptr)
                    ptr = ptr->_right;
                return ptr;
            }

            static _Const_Base_ptr _max(_Const_Base_ptr ptr) {
                while (ptr->_right != nullptr)
                    ptr = ptr->_right;
                return ptr;
            }

            bool _is_red() {
                return _color == _RB_tree_color::_red;
            }

            void _set_red() {
                _color = _RB_tree_color::_red;
            }

            void _set_black() {
                _color = _RB_tree_color::_black;
            }

            void _flip_color() {
                _color = (_color == _RB_tree_color::_black) ? _RB_tree_color::_red : _RB_tree_color::_black;
            }
    };
} // namespace MyStl
