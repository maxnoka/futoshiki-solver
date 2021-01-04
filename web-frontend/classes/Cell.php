<?php
class Cell {
    // object properties
    public $val;
    public $row_idx;
    public $col_idx;

    public function __construct($val, $row_idx, $col_idx) {
        $this->val = $val;
        $this->row_idx = $row_idx;
        $this->col_idx = $col_idx;
    }

    public function render() {
        include TEMPLATE_PATH . '/' . 'cell.html';
    }
}
?>