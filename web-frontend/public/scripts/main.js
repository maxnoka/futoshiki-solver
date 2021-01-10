$(document).ready(function(){
  initConstraintButtons();

  var clearButton = document.querySelector('#puzzle-clear-button');
  clearButton.onclick = function () {
    clearBoard();
    return false; // don't reload
  };
});

$(document).on('click', '#puzzle-solve-button', function(){
  var toSend = {"board_size" : 0,
                "rows" : [],
                "constraints": []};

  toSend.rows = getCellVals();
  toSend.board_size = toSend.rows.length;
  toSend.constraints = getConstraints();

  $.ajax({
    type: "POST",
    url: "solve.php",
    data: JSON.stringify(toSend),
    success: function(data){
        fillBoard(data);
    }
  });

  return false;
});

function initConstraintButtons() {
  var buttons = document.querySelectorAll('[id^="constraint-button-"]');

  buttons.forEach(puzzle_button => {
    var idString = puzzle_button.id.substring(18);
    var show = document.getElementById("constraint-display-" + idString);

    puzzle_button.onclick = function () {
      // cyclical order: < , > , [blank]
      if (show.innerText == "<") {
        show.innerText = ">";
      }
      else if (show.innerText == ">") {
        show.innerText = " ";
      }
      else {
        show.innerText = "<";
      }
    }
  });
};

function getCellVals() {
  var rows = document.querySelectorAll('[class^="puzzle-row-numbers"]');
  var rowsVals = []

  rows.forEach(row => {
    var cells = row.querySelectorAll('.puzzle-cell-number');
    var cellsVals = [];
    cells.forEach(cell => {
      var val = cell.querySelector('input').value;
      if (val == "") {
        cellsVals.push(0);
      }
      else {
        cellsVals.push(Number(val));
      }
    });
    rowsVals.push(cellsVals);
  });
  return rowsVals;
}

function getConstraints() {
  var constraints = document.querySelectorAll('[id^="constraint-display-"]');
  var out = [];

  constraints.forEach(constraint => {
    var idString = constraint.id.substring(19);  // leaves e.g. row_0_0
    var myRe = /(?<=_)[\d]+/gm;  // gets the digits after each underscore
    var matches = idString.match(myRe);

    var singleOut = {"rowIdx" : 0,
                     "colIdx": 0,
                     "operator" : "+",
                     "type" : "row"};

    var operator = constraint.innerText;
    if (operator == "<") {
      singleOut.operator = "<";
    }
    else if (operator == ">") {
      singleOut.operator = ">";
    }
    else {
      return;
    }
    singleOut.rowIdx = Number(matches[0]);
    singleOut.colIdx = Number(matches[1]);

    var type = idString.substring(0, 3);
    if (type == "row") {
      singleOut.type = "row";
    }
    else if (type == "col") {
      singleOut.type = "col";
    }
    else {
      return;
    }
    
    out.push(singleOut);
  });

  return out;
}

function clearBoard() {
  var rows = document.querySelectorAll('[class^="puzzle-row-numbers"]');

  rows.forEach(row => {
    var cells = row.querySelectorAll('.puzzle-cell-number');
    cells.forEach(cell => {
      cell.querySelector('input').value = null;
    });
  }); 

  var constraints = document.querySelectorAll('.puzzle-cell-constraint');
  constraints.forEach(constraint => {
    constraint.innerText = " ";
  });
};

function fillBoard(data) {
  var rows = document.querySelectorAll('[class^="puzzle-row-numbers"]');

  rows.forEach(function(row, rowIndex) {
    var cells = row.querySelectorAll('.puzzle-cell-number');
    cells.forEach(function(cell, cellIndex) {
      cell.querySelector('input').value = data[rowIndex][cellIndex];
    });
  }); 
};