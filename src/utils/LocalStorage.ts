export const saveToLocalStorage = (key: string, data: any) => {
  try {
    if (typeof window !== 'undefined') {
      const serializedData = JSON.stringify(data);
      localStorage.setItem(key, serializedData);
    }
  } catch (error) {
    console.error('Error saving to localStorage:', error);
  }
};

export const loadFromLocalStorage = (key: string) => {
  try {
    if (typeof window !== 'undefined') {
      const serializedData = localStorage.getItem(key);
      if (serializedData === null) {
        return undefined;
      }
      return JSON.parse(serializedData);
    }
  } catch (error) {
    console.error('Error loading from localStorage:', error);
    return undefined;
  }
};

