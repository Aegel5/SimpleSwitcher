namespace Images {

	// Обертка над Image, позволяющая получить изображение нужно размера.
	class ImageHandler {
		virtual Image GetImage(Vec_i2 desire_size) = 0;
	};
	class ImageHandlerBundle {
	};
	class ImageHandlerSVG {
	};
}
