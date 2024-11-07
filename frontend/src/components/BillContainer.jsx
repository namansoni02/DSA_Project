import React from 'react';

function BillContainer({ total }) {
  // Placeholder values
  const deliveryCharges = 50; // Example fixed value
  const serviceCharge = 20; // Example fixed value
  const tip = 30; // Example fixed value

  // Calculating the total amount including delivery charges, service charge, and tip
  const orderValue = total;
  const finalAmount = orderValue + deliveryCharges + serviceCharge + tip;

  return (
    <div className="bill-container">
      <h2>Total Bill</h2>
      <div className="bill-details">
        <div className="bill-row">
          <span>Order Value</span>
          <span>₹{orderValue.toFixed(2)}</span>
        </div>
        <div className="bill-row">
          <span>Delivery Charges</span>
          <span>₹{deliveryCharges.toFixed(2)}</span>
        </div>
        <div className="bill-row">
          <span>Service Charges</span>
          <span>₹{serviceCharge.toFixed(2)}</span>
        </div>
        <div className="bill-row">
          <span>Tip</span>
          <span>₹{tip.toFixed(2)}</span>
        </div>
        <div className="bill-row total">
          <span><strong>Total Amount</strong></span>
          <span><strong>₹{finalAmount.toFixed(2)}</strong></span>
        </div>
      </div>
    </div>
  );
}

export default BillContainer;
