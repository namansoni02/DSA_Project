import React from 'react';

function BillContainer({ total }) {
  return (
    <div className="bill-container">
      <h2>Total Bill</h2>
      <p>₹{total}</p> {/* Display total with 2 decimal points */}
    </div>
  );
}

export default BillContainer;