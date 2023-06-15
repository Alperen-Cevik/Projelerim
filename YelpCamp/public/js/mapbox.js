mapboxgl.accessToken = mapBoxToken;
const map = new mapboxgl.Map({
    container: 'map', // container ID
    style: 'mapbox://styles/mapbox/streets-v11', // style URL
    center: coordinates, // starting position [lng, lat]
    zoom: 10, // starting zoom
    projection: 'globe' // display the map as a 3D globe
});
map.on('style.load', () => {
    map.setFog({}); // Set the default atmosphere style
});

new mapboxgl.Marker()
    .setPopup(
        new mapboxgl.Popup({ offset: 20 })
            .setHTML(`<h4>${campgroundMetaData.title}</h4><p>${campgroundMetaData.location}</p>`)
    )
    .setLngLat(coordinates)
    .addTo(map)