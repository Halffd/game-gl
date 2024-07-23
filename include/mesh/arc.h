#ifndef CELL_ARC_H
#define CELL_ARC_H

#include "mesh.h"
#include <cmath>

namespace Cell
{
    class Arc : public Mesh
    {
    public:
        float innerRadius;
        float outerRadius;
        unsigned int segments;
        float startAngle;
        float endAngle;

        Arc() = default;

        // Helper function to convert hue to RGB
        static math::vec3 HUEtoRGB(float hue)
        {
            float r, g, b;
            int i = static_cast<int>(std::floor(hue * 6.0f));
            float f = hue * 6.0f - i;
            float p = 0.0f;
            float q = 1.0f - f;
            float t = f;
            switch (i % 6)
            {
            case 0:
                r = 1.0f;
                g = t;
                b = p;
                break;
            case 1:
                r = q;
                g = 1.0f;
                b = p;
                break;
            case 2:
                r = p;
                g = 1.0f;
                b = t;
                break;
            case 3:
                r = p;
                g = q;
                b = 1.0f;
                break;
            case 4:
                r = t;
                g = p;
                b = 1.0f;
                break;
            case 5:
                r = 1.0f;
                g = p;
                b = q;
                break;
            }
            return math::vec3(r, g, b);
        }

        Arc(float innerRadius, float outerRadius, unsigned int segments, float startAngle, float endAngle)
        {
            generateData(innerRadius, outerRadius, segments, startAngle, endAngle);
            Finalize();
        }

        void generateData(float innerRadius, float outerRadius, unsigned int segments, float startAngle, float endAngle)
        {
            float angleStep = (endAngle - startAngle) / segments;
            std::vector<math::vec3> positions;
            std::vector<math::vec3> colors;
            std::vector<unsigned int> indices;

            for (unsigned int i = 0; i <= segments; ++i)
            {
                float angle = startAngle + i * angleStep;
                float x1 = innerRadius * std::cos(angle);
                float y1 = innerRadius * std::sin(angle);
                float x2 = outerRadius * std::cos(angle);
                float y2 = outerRadius * std::sin(angle);

                positions.push_back(math::vec3(x1, y1, 0.0f));
                positions.push_back(math::vec3(x2, y2, 0.0f));

                float hue = (M_PI / 2.0f - angle) / (2.0f * M_PI);
                if (hue < 0.0f)
                    hue += 1.0f;
                math::vec3 color = HUEtoRGB(hue);

                colors.push_back(color);
                colors.push_back(color);
            }

            for (unsigned int i = 0; i < segments; ++i)
            {
                unsigned int offset = 2 * i;
                indices.push_back(0);
                indices.push_back(offset + 1);
                indices.push_back(offset + 2);

                indices.push_back(offset + 2);
                indices.push_back(offset + 1);
                indices.push_back(offset + 3);
            }

            Positions = positions;
            Colors = colors;
            Indices = indices;
        }
    };
}

#endif // CELL_ARC_H
