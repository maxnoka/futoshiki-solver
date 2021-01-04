$(document).ready(function(){
  initConstraintButtons();

  var clearButton = document.querySelector('#puzzle-clear-button');
  clearButton.onclick = function () {
    clearBoard();
    return false;
  };
});

/*
(function(doc, proto) {
  try { // check if browser supports :scope natively
    doc.querySelector(':scope body');
  } catch (err) { // polyfill native methods if it doesn't
    ['querySelector', 'querySelectorAll'].forEach(function(method) {
      var nativ = proto[method];
      proto[method] = function(selectors) {
        if (/(^|,)\s*:scope/.test(selectors)) { // only if selectors contains :scope
          var id = this.id; // remember current element id
          this.id = 'ID_' + Date.now(); // assign new unique id
          selectors = selectors.replace(/((^|,)\s*):scope/g, '$1#' + this.id); // replace :scope with #ID
          var result = doc[method](selectors);
          this.id = id; // restore previous id
          return result;
        } else {
          return nativ.call(this, selectors); // use native code for other selectors
        }
      }
    });
  }
})(window.document, Element.prototype);
*/

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
    var idString = constraint.id.substring(19); // leaves e.g. row_0_0
    var myRe = /(?<=_)[\d]+/gm;
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

$(document).on('click', '#puzzle-solve-button', function(){
  var toSend = {"board_size" : 0,
                "rows" : [],
                "constraints": []};

  toSend.rows = getCellVals();
  toSend.board_size = toSend.rows.length;
  toSend.constraints = getConstraints();

  $.ajax({
    type: "POST",
    url: "/solve.php",
    data: JSON.stringify(toSend), // serializes the form's elements.
    success: function(data){
        fillBoard(data); // show response from the php script.
    }
  });

  return false;
});