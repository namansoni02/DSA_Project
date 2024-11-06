import React from 'react';

function MenuItem({ item, isSelected, handleMenuItemSelection }) {
  const handleSelect = (e) => {
    handleMenuItemSelection(item.id, e.target.checked); // Pass checked status of checkbox
  };

  return (
    <div className="menu-item">
      <input
        type="checkbox" // Checkbox for multiple selection
        id={`menu-item-${item.id}`}
        name={`menu-${item.id}`} // Ensure unique name to allow independent checkbox behavior
        value={item.id}
        checked={isSelected}  // Determines if checkbox is selected
        onChange={handleSelect} // Handle change event to update state
      />
      <label htmlFor={`menu-item-${item.id}`}>
        {item.name} - ₹{item.price}
      </label>
    </div>
  );
}

export default MenuItem;
