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
        for ($rowIdx=0; $rowIdx < $board_size - 1; $rowIdx++) { 
            for ($colIdx=0; $colIdx < $board_size; $colIdx++) {    
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

        if(!isset($data->board_size) ||
           !isset($data->rows) ||
           !isset($data->constraints)){
            // set response code - 400 bad request
            http_response_code(400);
            echo json_encode(array("message" => "Unable to solve. Data is incomplete."));
            exit();
        }

        $size = filter_var(htmlspecialchars($data->board_size), FILTER_VALIDATE_INT);
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

        if (gettype($data->rows) != "array") {
            http_response_code(400);
            echo json_encode(array("message" => "Unable to solve. Invalid data (rows)"));
            exit();
        }
        
        if (sizeof($data->rows) != $size) {
            http_response_code(400);
            echo json_encode(array("message" => "Unable to solve. Invalid data (needs to be NxN shape, where N=board_size)"));
            exit();
        }

        foreach ($data->rows as $rowIdx => $row) {
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

            foreach ($row as $cellIdx => $val) {
                $validVal = filter_var(htmlspecialchars($val), FILTER_VALIDATE_INT);
                if ($validVal === false) {
                    http_response_code(400);
                    echo json_encode(array("message" => "Unable to solve. Board value is invalid (require int)."));
                    exit();
                }

                if ($validVal > $size || $validVal < 0) {
                    http_response_code(400);
                    echo json_encode(array("message" => "Unable to solve. Board value is invalid (require integer in range [0, board_size])."));
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
            if(!isset($constraintJson->colIdx) ||
               !isset($constraintJson->rowIdx) ||
               !isset($constraintJson->operator) ||
               !isset($constraintJson->type)){
                // set response code - 400 bad request 
                http_response_code(400);
                echo json_encode(array("message" => "Unable to solve. Data is incomplete. (constraints)"));
                exit();
            }

            $validColIdx = filter_var(htmlspecialchars($constraintJson->colIdx), FILTER_VALIDATE_INT);
            $validRowIdx = filter_var(htmlspecialchars($constraintJson->rowIdx), FILTER_VALIDATE_INT);

            if ($validColIdx === false || $validRowIdx === false) {
                http_response_code(400);
                echo json_encode(array("message" => "Unable to solve. Constraint or row indices invalid (require int)."));
                exit();
            }

            if ($constraintJson->operator == ">") {$validOperator = ">";}
            else if ($constraintJson->operator == "<") {$validOperator = "<";}
            else if ($constraintJson->operator == "+") {$validOperator = "+";}
            else {
                http_response_code(400);
                echo json_encode(array("message" => "Unable to solve. Constraint operator is invalid."));
                exit();
            }

            if ($constraintJson->type == "row") {$validType = "row";}
            else if ($constraintJson->type == "col") {$validType = "col";}
            else {
                http_response_code(400);
                echo json_encode(array("message" => "Unable to solve. Constraint type is invalid."));
                exit();
            }

            $eitherNegative = $validColIdx < 0 || $validRowIdx < 0;
            $eitherTooLarge = false;
            if (($validType == "row")) {
                if ($validColIdx >= $size - 1 || $validRowIdx >= $size) {
                    $eitherTooLarge = true;
                }
            }
            else {
                if ($validColIdx >= $size || $validRowIdx >= $size - 1) {
                    $eitherTooLarge = true;
                }
            }

            if ($eitherNegative || $eitherTooLarge) {
                http_response_code(400);
                echo json_encode(array("message" => "Unable to solve. Constraint indices invalid."));
                exit();
            }

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
        return $finalOutput;
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