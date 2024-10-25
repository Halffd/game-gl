
// Set up the WebGL context
const canvas = document.getElementById('myCanvas');
const gl = canvas.getContext('webgl');

// Set the viewport size
gl.viewport(0, 0, canvas.width, canvas.height);

// Vertex shader
const vertexShaderSource = `
      attribute vec3 position;
      uniform mat4 modelViewMatrix;
      uniform mat4 projectionMatrix;
      void main() {
        gl_Position = projectionMatrix * modelViewMatrix * vec4(position, 1.0);
      }
    `;

// Fragment shader
const fragmentShaderSource = `
      precision highp float;
      uniform float time;
      void main() {
        vec2 uv = gl_FragCoord.xy / vec2(1024.0, 1024.0);
        vec3 color = vec3(uv.x, uv.y + cos(time), 0.5 + 0.5 * sin(time));
        gl_FragColor = vec4(color, 1.0);
      }
    `;

// Create and compile the shaders
const vertexShader = gl.createShader(gl.VERTEX_SHADER);
gl.shaderSource(vertexShader, vertexShaderSource);
gl.compileShader(vertexShader);

const fragmentShader = gl.createShader(gl.FRAGMENT_SHADER);
gl.shaderSource(fragmentShader, fragmentShaderSource);
gl.compileShader(fragmentShader);

// Create the program and link the shaders
const program = gl.createProgram();
gl.attachShader(program, vertexShader);
gl.attachShader(program, fragmentShader);
gl.linkProgram(program);
gl.useProgram(program);


    // Set up the vertex data for a sphere
    const radius = 0.5;
    const latitudeBands = 150;
    const longitudeBands = 150;
    const vertexData = [];
    const indexData = [];

    for (let latNumber = 0; latNumber <= latitudeBands; latNumber++) {
      const theta = latNumber * Math.PI / latitudeBands;
      const sinTheta = Math.sin(theta);
      const cosTheta = Math.cos(theta);

      for (let longNumber = 0; longNumber <= longitudeBands; longNumber++) {
        const phi = longNumber * 2 * Math.PI / longitudeBands;
        const sinPhi = Math.sin(phi);
        const cosPhi = Math.cos(phi);

        const x = radius * cosPhi * sinTheta;
        const y = radius * cosTheta;
        const z = radius * sinPhi * sinTheta;

        vertexData.push(x, y, z);
      }
    }

    for (let latNumber = 0; latNumber < latitudeBands; latNumber++) {
      for (let longNumber = 0; longNumber < longitudeBands; longNumber++) {
        const first = (latNumber * (longitudeBands + 1)) + longNumber;
        const second = first + longitudeBands + 1;

        indexData.push(first);
        indexData.push(second);
        indexData.push(first + 1);

        indexData.push(second);
        indexData.push(second + 1);
        indexData.push(first + 1);
      }
    }
// Create the vertex buffer
const vertexBuffer = gl.createBuffer();
gl.bindBuffer(gl.ARRAY_BUFFER, vertexBuffer);
gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vertexData), gl.STATIC_DRAW);

// 2. Create EBO
const indexBuffer = gl.createBuffer();
gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, indexBuffer);
gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, new Uint16Array(indexData), gl.STATIC_DRAW);

// Get the location of the position attribute in the shader
const positionLocation = gl.getAttribLocation(program, 'position');
gl.enableVertexAttribArray(positionLocation);
gl.vertexAttribPointer(positionLocation, 3, gl.FLOAT, false, 0, 0);

// Get the locations of the uniform variables in the shader
const modelViewMatrixLocation = gl.getUniformLocation(program, 'modelViewMatrix');
const projectionMatrixLocation = gl.getUniformLocation(program, 'projectionMatrix');
const timeLocation = gl.getUniformLocation(program, 'time');

// Set up the projection matrix
const fieldOfView = 45 * Math.PI / 180; // in radians
const aspect = canvas.width / canvas.height;
const zNear = 0.1;
const zFar = 100.0;
const projectionMatrix = mat4.create();
mat4.perspective(projectionMatrix, fieldOfView, aspect, zNear, zFar);

// Set the clear color to black
gl.clearColor(0.0, 0.0, 0.0, 1.0);

// Main rendering loop
let lastTime = performance.now();
function render() {
    const currentTime = performance.now();
    const deltaTime = (currentTime - lastTime) / 1000; // in seconds
    lastTime = currentTime;

    // Update the model-view matrix
    const modelViewMatrix = mat4.create();
    mat4.translate(modelViewMatrix, modelViewMatrix, [0, 0, -1.5]);
    mat4.rotateY(modelViewMatrix, modelViewMatrix, deltaTime * 0.2)
    // Update the uniform variables
    gl.uniformMatrix4fv(modelViewMatrixLocation, false, modelViewMatrix);
    gl.uniformMatrix4fv(projectionMatrixLocation, false, projectionMatrix);
    gl.uniform1f(timeLocation, currentTime / 1000);

    // Clear the color and depth buffers
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

    // Draw the sphere
    gl.drawElements(gl.TRIANGLES, indexData.length, gl.UNSIGNED_SHORT, 0);

    // Request the next frame
    requestAnimationFrame(render);
}

// Start the rendering loop
render();