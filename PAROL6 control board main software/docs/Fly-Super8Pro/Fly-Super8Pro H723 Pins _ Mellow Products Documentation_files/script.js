// Переменные для управления изображением
let currentScale = 1;
let currentTranslate = { x: 0, y: 0 };
let isDragging = false;
let startPos = { x: 0, y: 0 };
let currentImage = null;
let isSchematicMode = false;

// Получаем элементы
const imageViewer = document.getElementById('image-viewer');
const photoImage = document.getElementById('photo-image');
const schematicImage = document.getElementById('schematic-image');
const zoomPercent = document.getElementById('zoom-percent');
const zoomLevel = document.getElementById('zoom-level');
const dragHint = document.getElementById('drag-hint');

// Функция для обновления трансформации изображения
function updateImageTransform() {
    if (currentImage) {
        currentImage.style.transform = `translate(${currentTranslate.x}px, ${currentTranslate.y}px) scale(${currentScale})`;
        zoomPercent.textContent = `${Math.round(currentScale * 100)}%`;
    }
}

// Функция для зума
function zoom(factor, clientX = null, clientY = null) {
    const oldScale = currentScale;
    currentScale *= factor;
    
    // Ограничиваем масштаб
    if (currentScale < 0.5) currentScale = 0.5;
    if (currentScale > 5) currentScale = 5;
    
    // Если переданы координаты, зумируем относительно курсора
    if (clientX !== null && clientY !== null && isSchematicMode) {
        const rect = imageViewer.getBoundingClientRect();
        const mouseX = clientX - rect.left;
        const mouseY = clientY - rect.top;
        
        // Вычисляем смещение для зума относительно курсора
        currentTranslate.x = mouseX - (mouseX - currentTranslate.x) * (currentScale / oldScale);
        currentTranslate.y = mouseY - (mouseY - currentTranslate.y) * (currentScale / oldScale);
    }
    
    updateImageTransform();
    updateZoomControls();
}

// Функции зума кнопками
function zoomIn() {
    zoom(1.2);
}

function zoomOut() {
    zoom(0.8);
}

function resetZoom() {
    currentScale = 1;
    currentTranslate = { x: 0, y: 0 };
    updateImageTransform();
    updateZoomControls();
}

// Обновление видимости элементов управления зумом
function updateZoomControls() {
    if (currentScale > 1) {
        zoomLevel.classList.remove('d-none');
    } else {
        zoomLevel.classList.add('d-none');
    }
}

// Обработчики событий мыши для перетаскивания
function handleMouseDown(e) {
    if (!isSchematicMode) return;
    
    isDragging = true;
    startPos = { x: e.clientX - currentTranslate.x, y: e.clientY - currentTranslate.y };
    imageViewer.classList.add('grabbing');
    e.preventDefault();
}

function handleMouseMove(e) {
    if (!isDragging || !isSchematicMode) return;
    
    currentTranslate.x = e.clientX - startPos.x;
    currentTranslate.y = e.clientY - startPos.y;
    updateImageTransform();
    e.preventDefault();
}

function handleMouseUp() {
    isDragging = false;
    imageViewer.classList.remove('grabbing');
}

// Обработчик колеса мыши
function handleWheel(e) {
    if (!isSchematicMode) return;
    
    e.preventDefault();
    const delta = e.deltaY > 0 ? 0.9 : 1.1;
    zoom(delta, e.clientX, e.clientY);
}

// Функция для показа фотографии
function showPhoto() {
    photoImage.classList.remove('d-none');
    schematicImage.classList.add('d-none');
    
    document.getElementById('photo-info').classList.remove('d-none');
    document.getElementById('schematic-info').classList.add('d-none');
    
    document.getElementById('photo-description').classList.remove('d-none');
    document.getElementById('schematic-description').classList.add('d-none');
    
    // Обновляем активные кнопки
    document.getElementById('btn-photo').classList.add('active', 'btn-primary');
    document.getElementById('btn-photo').classList.remove('btn-outline-primary');
    
    document.getElementById('btn-schematic').classList.remove('active', 'btn-primary');
    document.getElementById('btn-schematic').classList.add('btn-outline-primary');
    
    // Устанавливаем текущее изображение
    currentImage = photoImage;
    isSchematicMode = false;
    dragHint.classList.add('d-none');
    
    // Сбрасываем трансформации
    resetZoom();
    
    // Убираем обработчики перетаскивания
    imageViewer.style.cursor = 'default';
}

// Функция для показа схемы
function showSchematic() {
    photoImage.classList.add('d-none');
    schematicImage.classList.remove('d-none');
    
    document.getElementById('photo-info').classList.add('d-none');
    document.getElementById('schematic-info').classList.remove('d-none');
    
    document.getElementById('photo-description').classList.add('d-none');
    document.getElementById('schematic-description').classList.remove('d-none');
    
    // Обновляем активные кнопки
    document.getElementById('btn-schematic').classList.add('active', 'btn-primary');
    document.getElementById('btn-schematic').classList.remove('btn-outline-primary');
    
    document.getElementById('btn-photo').classList.remove('active', 'btn-primary');
    document.getElementById('btn-photo').classList.add('btn-outline-primary');
    
    // Устанавливаем текущее изображение
    currentImage = schematicImage;
    isSchematicMode = true;
    dragHint.classList.remove('d-none');
    
    // Сбрасываем трансформации
    resetZoom();
    
    // Добавляем курсор для перетаскивания
    imageViewer.style.cursor = 'grab';
}

// Инициализация при загрузке страницы
document.addEventListener('DOMContentLoaded', function() {
    // Показываем фотографию по умолчанию
    showPhoto();
    
    // Добавляем обработчики событий
    imageViewer.addEventListener('mousedown', handleMouseDown);
    document.addEventListener('mousemove', handleMouseMove);
    document.addEventListener('mouseup', handleMouseUp);
    imageViewer.addEventListener('wheel', handleWheel);
    
    // Для touch-устройств
    imageViewer.addEventListener('touchstart', function(e) {
        if (!isSchematicMode) return;
        e.preventDefault();
        const touch = e.touches[0];
        handleMouseDown(touch);
    });
    
    imageViewer.addEventListener('touchmove', function(e) {
        if (!isSchematicMode) return;
        e.preventDefault();
        const touch = e.touches[0];
        handleMouseMove(touch);
    });
    
    imageViewer.addEventListener('touchend', handleMouseUp);
    
    // Проверяем загрузку SVG
    schematicImage.addEventListener('error', function() {
        console.log('SVG не загружается через тег <img>');
    });
    
    schematicImage.addEventListener('load', function() {
        console.log('SVG успешно загружен');
    });
});

// Плавная прокрутка
document.querySelectorAll('a[href^="#"]').forEach(anchor => {
    anchor.addEventListener('click', function (e) {
        e.preventDefault();
        
        const targetId = this.getAttribute('href');
        if(targetId === '#') return;
        
        const targetElement = document.querySelector(targetId);
        if(targetElement) {
            window.scrollTo({
                top: targetElement.offsetTop - 80,
                behavior: 'smooth'
            });
        }
    });
});

// Подсветка активного раздела
window.addEventListener('scroll', function() {
    let current = '';
    const sections = document.querySelectorAll('section[id]');
    
    sections.forEach(section => {
        const sectionTop = section.offsetTop;
        const sectionHeight = section.clientHeight;
        if(scrollY >= (sectionTop - 100)) {
            current = section.getAttribute('id');
        }
    });
    
    document.querySelectorAll('.nav-link').forEach(link => {
        link.classList.remove('active');
        if(link.getAttribute('href') === `#${current}`) {
            link.classList.add('active');
        }
    });
});

// Горячие клавиши
document.addEventListener('keydown', function(e) {
    if (e.ctrlKey && isSchematicMode) {
        switch(e.key) {
            case '+':
            case '=':
                e.preventDefault();
                zoomIn();
                break;
            case '-':
                e.preventDefault();
                zoomOut();
                break;
            case '0':
                e.preventDefault();
                resetZoom();
                break;
        }
    }
});