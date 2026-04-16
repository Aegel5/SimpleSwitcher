#pragma once

#include "backends_layer.h"

namespace Images {
	namespace details {
		class TextureImpl {
		public:
			ImageImpl img;
			ImTextureID pTexture = 0;
			bool IsOk() {
				return pTexture != 0;
			}
			void clear() {
				if (pTexture) {
					ImBackends::CleanupTexture(pTexture);
					pTexture = 0;
				}
			}
			~TextureImpl() {
				clear();
			}
		};
	}

	using ShaderResource = std::shared_ptr<details::TextureImpl>;

	inline ShaderResource ImageToShaderConsume(Image image) {
		ShaderResource res = MAKE_SHARED(res);
		res->img = *image;
		image->data = 0; // move ownership
		auto* cur = &res->img;
		if (!cur->IsOk()) return res;
		res->pTexture = ImBackends::LoadTexture_RGBA8(cur->data, cur->width, cur->height);
		cur->clear(); // не требуется
		return res;
	}
}
