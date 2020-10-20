#pragma once
#include <rendering/detail/engine_include.hpp>

namespace args::rendering
{
	struct renderbuffer
	{
		app::gl_id id;

		renderbuffer(GLenum internalformat, math::ivec2 resolution, int samples = 0)
		{
			glGenRenderbuffers(1, &id);
			if (samples > 0)
				glNamedRenderbufferStorageMultisample(id, samples, internalformat, resolution.x, resolution.y);
			else
				glNamedRenderbufferStorage(id, internalformat, resolution.x, resolution.y);
		}

		renderbuffer(GLenum internalformat, int width, int height, int samples = 0)
		{
			glGenRenderbuffers(1, &id);
			if (samples > 0)
				glNamedRenderbufferStorageMultisample(id, samples, internalformat, width, height);
			else
				glNamedRenderbufferStorage(id, internalformat, width, height);
		}

		~renderbuffer()
		{
			glDeleteRenderbuffers(1, &id);
		}

		void bind()
		{
			glBindRenderbuffer(GL_RENDERBUFFER, id);
		}

	};
}
