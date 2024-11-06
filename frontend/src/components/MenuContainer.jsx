import React from 'react';
import MenuItem from './MenuItem';

function MenuContainer({ menuItems, selectedMenuItems, handleMenuItemSelection }) {
  return (
    <div className="menu-container">
      <h1 id = 'restaurant'>Menu</h1>
      {menuItems.length > 0 ? (
        menuItems.map((item) => (
          <MenuItem
            key={item.id}
            item={item}
            isSelected={selectedMenuItems.includes(item.id)}  // check if item is in selectedMenuItems
            handleMenuItemSelection={handleMenuItemSelection} // Pass function here
          />
        ))
      ) : (
        <p>Select a restaurant to see the menu.</p>
      )}
    </div>
  );
}

export default MenuContainer;
