import React from 'react';

function DropdownMenu({ setSelectedRestaurant }) {
  const handleChange = (event) => {
    setSelectedRestaurant(event.target.value);
  };

  return (
    <div>
      <h1 id='restaurant'>Restaurant</h1>
      <select className="dropdown" onChange={handleChange}>
        <option value="">Select a Restaurant</option>
        <option value="McDonalds">McDonalds</option>
        <option value="Pizza Hut">Pizza Hut</option>
        <option value="Starbucks">Starbucks</option>
        <option value="Maharaja Feast">Maharaja Feast</option>
        <option value="Royal Tandoor">Royal Tandoor</option>
        <option value="Biryani Bazaar">Biryani Bazaar</option>
        <option value="Spice Junction">Spice Junction</option>
        <option value="Shucks The Cafe">Shucks The Cafe</option>
      </select>
    </div>
  );
}

export default DropdownMenu;