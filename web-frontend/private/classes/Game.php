<?php
require_once(__DIR__ . "./../config/config.php");

require_once('Row.php');
require_once('Constraint.php');

class Game {
    public $board_size;
    public $rows = array();
    public $constraints = array("row" => array(), "col" => array());

    public function __construct($board_size) {
        $this->board_size = $board_size;

        // have $board_size # of rows
        // comprised of $board_size # of cols
        // all set to be empty (zero)
        for ($i=0; $i < $board_size; $i++) { 
            $this->rows[] = new Row($board_size, $i);
        }
        
        // create a constraint of each set of adjacent cells
        // all set with constraint type "+" (no constraint)  

        // the constraints between cells of the same row
        for ($rowIdx=0; $rowIdx < $board_size; $rowIdx++) { 
            for ($colIdx=0; $colIdx < $board_size - 1; $colIdx++) {
                $new = new Constraint($colIdx, $rowIdx, "+", "row");
                $this->constraints["row"][$rowIdx][] = $new;
            }
        }

        // the constraints between cells of different rows
        for ($rowIdx=0; $rowIdx < $board_size; $rowIdx++) { 
            for ($colIdx=0; $colIdx < $board_size - 1; $colIdx++) {    
                $new = new Constraint($colIdx, $rowIdx, "+", "col");
                $this->constraints["col"][$colIdx][] = $new;
            }
        }
    }

    // expect e.g. 
    //
    //      {
    //       "board_size": 3,
    //       "rows": [[1,0,2],[0,1,0],[0,0,2]],
    //       "constraints":
    //          [{"rowIdx":1,"colIdx":2,"operator":">","type":"col"},
    //           {"rowIdx":1,"colIdx":2,"operator":"<","type":"row"}]
    //      }
    // 
    // Requirements:
    //  - valid json
    //  - all three fields shown above required
    //  - board size is integer in range [2, 7], 
    //  - rows needs to be size N array of size N arrays (where N is board size)
    //  - each entry in rows needs to be integer in range [0, N-1]
    //  - constraints needs to be an array
    //  - all four fields shown above required
    //  - rowIdx needs to be integer in range [0, N-1] or [0, N-2] for "row"/"col" type respectiely
    //  - colIdx needs to be integer in range [0, N-2] or [0, N-1] for "row"/"col" type respectiely
    //  - constraint operator needs to be in set ["<", ">", "+"] ("+" is no constraint)
    //  - constraint type needs to be in set ["row"], ["col"]
    public static function createFromJson($json) {
        $data = json_decode($json);

        if ($data === null) {
            http_response_code(400);
            echo json_encode(array("message" => "Unable to solve. Invalid json."));
            exit();
        }

        if(!isset($data->grid_size) ||
           !isset($data->cells) ||
           !isset($data->constraints)){
            // set response code - 400 bad request
            http_response_code(400);
            echo json_encode(array("message" => "Unable to solve. Data is incomplete."));
            exit();
        }

        $size = filter_var(htmlspecialchars($data->grid_size), FILTER_VALIDATE_INT);
        if ($size === false) {
            http_response_code(400);
            echo json_encode(array("message" => "Unable to solve. Board size is invalid (require int)."));
            exit();
        }

        if ($size > 7 || $size < 2) {
            http_response_code(400);
            echo json_encode(array("message" => "Unable to solve. Board size is invalid (needs to be in range [2,7])."));
            exit();
        }

        $obj = new Game($size);

        if (gettype($data->cells) != "array") {
            http_response_code(400);
            echo json_encode(array("message" => "Unable to solve. Invalid data (cells)"));
            exit();
        }

        if (sizeof($data->cells) != $size) {
            http_response_code(400);
            echo json_encode(array("message" => "Unable to solve. Invalid data (needs to be NxN shape, where N=board_size)"));
            exit();
        }

        $lookup = array();
        foreach ($data->cells as $rowIdx => $row) {
            if (gettype($row) != "array") {
                http_response_code(400);
                echo json_encode(array("message" => "Unable to solve. Invalid data (row within rows)"));
                exit();
            }

            if (sizeof($row) != $size) {
                http_response_code(400);
                echo json_encode(array("message" => "Unable to solve. Invalid data (needs to be NxN shape)"));
                exit();
            }

            foreach ($row as $cellIdx => $cellObj) {
                if (gettype($cellObj) != "object") {
                    http_response_code(400);
                    echo json_encode(array("message" => "Unable to solve. Invalid data (cell object)"));
                    exit();
                }

                if(!isset($cellObj->val)
                   || !isset($cellObj->cell_id)){
                    // set response code - 400 bad request
                    http_response_code(400);
                    echo json_encode(array("message" => "Unable to solve. Cell data is incomplete."));
                    exit();
                }
                $lookup[$cellObj->cell_id]["row"] = $rowIdx;
                $lookup[$cellObj->cell_id]["col"] = $cellIdx;

                $val = $cellObj->val;
                $validVal = filter_var(htmlspecialchars($val), FILTER_VALIDATE_INT);
                if ($validVal === false) {
                    http_response_code(400);
                    echo json_encode(array("message" => "Unable to solve. Cell value is invalid (require int)."));
                    exit();
                }

                if ($validVal > $size || $validVal < 0) {
                    http_response_code(400);
                    echo json_encode(array("message" => "Unable to solve. Cell value is invalid (require integer in range [0, board_size])."));
                    exit();
                }

                $obj->rows[$rowIdx]->cells[$cellIdx]->val = $validVal;
            }
        }

        if (gettype($data->constraints) != "array") {
            http_response_code(400);
            echo json_encode(array("message" => "Unable to solve. Invalid data (constraints)"));
            exit();
        }

        foreach ($data->constraints as $constraintIdx => $constraintJson) {
            if (gettype($constraintJson) != "object") {
                http_response_code(400);
                echo json_encode(array("message" => "Unable to solve. Invalid data (constraint object)"));
                exit();
            }

            if(!isset($constraintJson->cells) ||
               !isset($constraintJson->operator) ||
               !isset($constraintJson->type)){
                // set response code - 400 bad request 
                http_response_code(400);
                echo json_encode(array("message" => "Unable to solve. Data is incomplete. (constraints)"));
                exit();
            }

            if (gettype($constraintJson->cells) != "array") {
                http_response_code(400);
                echo json_encode(array("message" => "Unable to solve. Invalid data (cells array in constraint object)"));
                exit();
            }

            if (sizeof($constraintJson->cells) != 2) {
                http_response_code(400);
                echo json_encode(array("message" => "Unable to solve. Invalid data (constraints need to have exactly two cells)"));
                exit();
            }

            foreach ($constraintJson->cells as $cellIdx => $cellId) {
                if (!key_exists($cellId, $lookup)) {
                    http_response_code(400);
                    echo json_encode(array("message" => "Unable to solve.  Invalid data (constraints reference non existant cells)"));
                    exit();
                }
            }
            $rowIdx_lhs = $lookup[$constraintJson->cells[0]]["row"];
            $colIdx_lhs = $lookup[$constraintJson->cells[0]]["col"];
            $rowIdx_rhs = $lookup[$constraintJson->cells[1]]["row"];
            $colIdx_rhs = $lookup[$constraintJson->cells[1]]["col"];


            $diffX = $colIdx_lhs - $colIdx_rhs;
            $diffY = $rowIdx_lhs - $rowIdx_rhs;

            // var_dump($diffX);
            // var_dump($diffY);

            if (abs($diffX) == 1 && $diffY == 0) {
                $validType = "row";
                $validColIdx = $colIdx_rhs + $diffX;
                $validRowIdx = $rowIdx_lhs;
            }
            else if (abs($diffY) == 1 && $diffX == 0) {
                $validType = "col";
                $validColIdx = $colIdx_lhs;
                $validRowIdx = $rowIdx_rhs + $diffY;
            }
            else {
                http_response_code(400);
                echo json_encode(array("message" => "Unable to solve.  Invalid data (constraints cells not adjacent)"));
                exit();
            }

            if ($constraintJson->operator == ">") {
                if ($diffX < 0 || $diffY < 0) {
                    $validOperator = "<";
                }
                $validOperator = ">";
            }
            else if ($constraintJson->operator == "<") {
                if ($diffX < 0 || $diffY < 0) {
                    $validOperator = ">";
                }
                $validOperator = "<";
            }
            else {
                http_response_code(400);
                echo json_encode(array("message" => "Unable to solve. Constraint operator is invalid."));
                exit();
            }

/*
            var_dump($validColIdx);
            var_dump($validRowIdx);
            var_dump($validOperator);
            var_dump($validType);
*/

            $obj->addConstraint($validColIdx, $validRowIdx, $validOperator, $validType);
        }
        return $obj;
    }

    public function render() {
        include TEMPLATE_PATH . '/' . 'game.html';
    }

    public function solve() {
        // $command is e.g.
        //      ./futoshiki-solver "0,0,0,0  
        //      0,0,0,2 
        //      0,0,0,0
        //      0,0,0,0" "0,0:0,1:>
        //      0,2:0,3:<
        //      1,1:1,2:<
        //      2,2:2,3:<
        //      3,0:3,1:>"

/*
        $command = BIN_PATH . "/futoshiki-solver ";
        $command .= "\"" . $this->serializeBoard() . "\" ";
        $command .= "\"" . $this->serializeConstraints() . "\"";

        $output=null;
        $retval=null;
        exec($command, $output, $retval);

        if ($retval == 2) {
            return "not valid";
        }

        $finalOutput = array();
        foreach ($output as $outRowIdx => $outRow) {
            $finalOutput[] = explode(",", $outRow);

        }
*/
    var_dump($result);
        
        return $this->serializeGrid();
    }

    public function addConstraint($colIdx, $rowIdx, $operator, $type) {
        if ($type == "row") {
            $this->constraints["row"][$rowIdx][$colIdx]->operator = $operator;
        }
        else {
            $this->constraints["col"][$colIdx][$rowIdx]->operator = $operator;
        }
    }

    public function serializeBoard() {
        $rowStrings = array();
        foreach ($this->rows as $rowIdx => $row) {
            $rowStrings[] = $row->serializeRow();
        }
        return implode("\n", $rowStrings);
    }

    public function serializeConstraints() {
        $constraintStrings = array();

        foreach ($this->constraints["row"] as $i => $constraintRow) {
            foreach ($constraintRow as $k => $constraint) {
                if ($constraint->operator != "+") {
                    $constraintStrings[] = $constraint->serializeConstraint();
                }
            }
        }

        foreach ($this->constraints["col"] as $i => $constraintCol) {
            foreach ($constraintCol as $k => $constraint) {
                if ($constraint->operator != "+") {
                    $constraintStrings[] = $constraint->serializeConstraint();
                }
            }
        }

        return implode("\n", $constraintStrings);
    }
}
?>
