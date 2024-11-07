import { useState, useEffect } from "react";
import "./App.css";
import Map from "./components/Map";
import DropdownMenu from './components/DropdownMenu';
import MenuContainer from './components/MenuContainer';
import BillContainer from "./components/BillContainer";
import menuData from './data/menuData.json'; 
function App() {
  const [selectedRestaurant, setSelectedRestaurant] = useState('');
  const [menuItems, setMenuItems] = useState([]);
  const [selectedMenuItems, setSelectedMenuItems] = useState([]); // Array to store selected menu items

  useEffect(() => {
  if (selectedRestaurant) {
    // Fetch menu items based on the selected restaurant
    const menu = menuData[selectedRestaurant] || [];
    console.log("Selected Menu: ", menu);  // Log the menu to check
    setMenuItems(menu);
  } else {
    setMenuItems([]);
  }
  }, [selectedRestaurant]);

  const handleMenuItemSelection = (itemId, isChecked) => {
    if (isChecked) {
      // Add item to selected list if checked
      setSelectedMenuItems((prevItems) => [...prevItems, itemId]);
    } else {
      // Remove item from selected list if unchecked
      setSelectedMenuItems((prevItems) => prevItems.filter((id) => id !== itemId));
    }
  };

  const calculateTotal = () => {
    return selectedMenuItems.reduce((total, itemId) => {
      const item = menuItems.find(item => item.id === itemId);
      return total + (item ? item.price : 0);
    }, 0);
  };

  return (
    <>
      <h1 className="title">YumBoXX</h1>
      <div className="container">
        <div className = 'left-container'>
          <div className="map-container">
            <Map />
          </div>
          <div className="delivery-container">

          </div>
        </div>
        <div className="sidebar">
          <DropdownMenu setSelectedRestaurant={setSelectedRestaurant} />
          <MenuContainer
              menuItems={menuItems}
              selectedMenuItems={selectedMenuItems}
              handleMenuItemSelection={handleMenuItemSelection} // Pass the function to update selected items
          />
          <BillContainer 
              total={calculateTotal()}
          />
          <div className="pay-now-container">
            <button className="pay-now-btn">
                Pay Now
            </button>
          </div>
        </div>
      </div>
    </>
  );
}

export default App;
